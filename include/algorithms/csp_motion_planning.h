#pragma once

#include <cstdint>
#include <vector>
#include <cmath>

class CSPMotionPlanning {
public:
    // Singleton pattern
    static CSPMotionPlanning& getInstance() {
        static CSPMotionPlanning instance;
        return instance;
    }

    // Motion parameters structure
    struct MotionParams {
        int32_t target_position;    // Target position
        int32_t max_velocity;       // Maximum velocity
        int32_t acceleration;       // Acceleration
        int32_t deceleration;      // Deceleration
        int32_t current_position;   // Current position
        int32_t current_velocity;   // Current velocity
    };

    // Motion state
    struct MotionState {
        int32_t position;          // Position
        int32_t velocity;          // Velocity
        bool is_completed;         // Whether motion is completed
    };

    // Initialize motion planning
    void init(const MotionParams& params);

    // Calculate next position point (called once per cycle)
    MotionState calculateNextState(int cycle_time_us);

    // Check if motion is completed
    bool isMotionCompleted() const { return motion_completed; }

private:
    CSPMotionPlanning() = default;
    
    // Motion phases
    enum class Phase {
        ACCELERATION,
        CONSTANT_VELOCITY,
        DECELERATION,
        COMPLETED
    };

    MotionParams params;           // Motion parameters
    Phase current_phase;           // Current motion phase
    bool motion_completed;         // Motion completion flag
    
    double current_position;       // Current position
    double current_velocity;       // Current velocity
    double current_acceleration;   // Current acceleration
    double distance_to_go;         // Remaining distance
    double total_distance;         // Total distance
    double decel_start_pos;       // Position to start deceleration
    double accel_end_pos;         // Position where acceleration ends
    
    // Additional helper functions
    void calculateTrajectoryProfile();    // Calculate complete trajectory profile
    double calculateStoppingDistance(double velocity, double deceleration);  // Calculate stopping distance
    bool needDeceleration();             // Determine if deceleration is needed
    void updatePhase();                  // Update motion phase
};
