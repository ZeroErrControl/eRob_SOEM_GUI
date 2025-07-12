#include "csp_motion_planning.h"
#include <cmath>

void CSPMotionPlanning::init(const MotionParams& params) {
    this->params = params;
    this->current_phase = Phase::ACCELERATION;
    this->motion_completed = false;
    
    this->current_position = params.current_position;
    this->current_velocity = params.current_velocity;
    this->current_acceleration = 0.0; // Added: current acceleration, initialized to 0
    this->total_distance = std::abs(params.target_position - params.current_position);
    this->distance_to_go = params.target_position - params.current_position;
    
    // Calculate trajectory profile
    calculateTrajectoryProfile();
}

void CSPMotionPlanning::calculateTrajectoryProfile() {
    // Calculate distance needed to accelerate to max velocity
    double accel_distance = (params.max_velocity * params.max_velocity - 
                           current_velocity * current_velocity) / (2.0 * params.acceleration);
    
    // Calculate distance needed to decelerate from max velocity to stop
    double decel_distance = (params.max_velocity * params.max_velocity) / (2.0 * params.deceleration);
    
    // If total distance is less than sum of acceleration and deceleration distances, adjust max velocity
    if (total_distance < (accel_distance + decel_distance)) {
        double v_max = std::sqrt(
            (2.0 * total_distance * params.acceleration * params.deceleration) /
            (params.acceleration + params.deceleration)
        );
        params.max_velocity = static_cast<int32_t>(v_max);
        
        accel_distance = (v_max * v_max - current_velocity * current_velocity) / 
                        (2.0 * params.acceleration);
        decel_distance = (v_max * v_max) / (2.0 * params.deceleration);
    }
    
    // Calculate key position points
    if (distance_to_go >= 0) {
        accel_end_pos = current_position + accel_distance;
        decel_start_pos = params.target_position - decel_distance;
    } else {
        accel_end_pos = current_position - accel_distance;
        decel_start_pos = params.target_position + decel_distance;
    }
}

CSPMotionPlanning::MotionState CSPMotionPlanning::calculateNextState(int cycle_time_us) {
    double dt = cycle_time_us / 1e6;
    MotionState state;

    if (motion_completed) {
        state.position = params.target_position;
        state.velocity = 0;
        state.is_completed = true;
        return state;
    }

    double direction = (params.target_position >= params.current_position) ? 1.0 : -1.0;

    if (current_phase == Phase::ACCELERATION) {
        current_velocity += direction * params.acceleration * dt;
        if (std::abs(current_velocity) > std::abs(params.max_velocity)) {
            current_velocity = direction * params.max_velocity;
        }
        current_position += current_velocity * dt;

        // Reached acceleration phase end, enter constant velocity phase
        if ((direction > 0 && current_position >= accel_end_pos) ||
            (direction < 0 && current_position <= accel_end_pos)) {
            current_phase = Phase::CONSTANT_VELOCITY;
            current_position = accel_end_pos;
            current_velocity = direction * params.max_velocity;
        }
    }
    else if (current_phase == Phase::CONSTANT_VELOCITY) {
        current_position += current_velocity * dt;

        // Reached deceleration phase start, enter deceleration phase
        if ((direction > 0 && current_position >= decel_start_pos) ||
            (direction < 0 && current_position <= decel_start_pos)) {
            current_phase = Phase::DECELERATION;
            current_position = decel_start_pos;
        }
    }
    else if (current_phase == Phase::DECELERATION) {
        current_velocity -= direction * params.deceleration * dt;
        // Prevent velocity reversal
        if (direction * current_velocity < 0) {
            current_velocity = 0;
        }
        current_position += current_velocity * dt;

        // Reached target point
        if ((direction > 0 && current_position >= params.target_position) ||
            (direction < 0 && current_position <= params.target_position)) {
            current_position = params.target_position;
            current_velocity = 0;
            motion_completed = true;
            current_phase = Phase::COMPLETED;
        }
    }

    distance_to_go = params.target_position - current_position;

    state.position = static_cast<int32_t>(current_position);
    state.velocity = static_cast<int32_t>(current_velocity);
    state.is_completed = motion_completed;
    return state;
}

void CSPMotionPlanning::updatePhase() {
    if (std::abs(distance_to_go) < 1.0 && std::abs(current_velocity) < 1.0) {
        current_phase = Phase::COMPLETED;
        return;
    }
    
    if (distance_to_go >= 0) {
        if (current_position < accel_end_pos) {
            current_phase = Phase::ACCELERATION;
        } else if (current_position >= decel_start_pos) {
            current_phase = Phase::DECELERATION;
        } else {
            current_phase = Phase::CONSTANT_VELOCITY;
        }
    } else {
        if (current_position > accel_end_pos) {
            current_phase = Phase::ACCELERATION;
        } else if (current_position <= decel_start_pos) {
            current_phase = Phase::DECELERATION;
        } else {
            current_phase = Phase::CONSTANT_VELOCITY;
        }
    }
}