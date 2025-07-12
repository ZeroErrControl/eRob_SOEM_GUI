/****************************************************************************
** Meta object code from reading C++ file 'ethercat_thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/ethercat_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ethercat_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EtherCATThread_t {
    QByteArrayData data[17];
    char stringdata0[192];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EtherCATThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EtherCATThread_t qt_meta_stringdata_EtherCATThread = {
    {
QT_MOC_LITERAL(0, 0, 14), // "EtherCATThread"
QT_MOC_LITERAL(1, 15, 12), // "dataReceived"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 8), // "position"
QT_MOC_LITERAL(4, 38, 8), // "velocity"
QT_MOC_LITERAL(5, 47, 6), // "torque"
QT_MOC_LITERAL(6, 54, 13), // "statusUpdated"
QT_MOC_LITERAL(7, 68, 11), // "MotorStatus"
QT_MOC_LITERAL(8, 80, 6), // "status"
QT_MOC_LITERAL(9, 87, 11), // "errorSignal"
QT_MOC_LITERAL(10, 99, 5), // "error"
QT_MOC_LITERAL(11, 105, 15), // "messageReceived"
QT_MOC_LITERAL(12, 121, 3), // "msg"
QT_MOC_LITERAL(13, 125, 17), // "motorStateChanged"
QT_MOC_LITERAL(14, 143, 15), // "newDataReceived"
QT_MOC_LITERAL(15, 159, 18), // "motorStatusUpdated"
QT_MOC_LITERAL(16, 178, 13) // "errorOccurred"

    },
    "EtherCATThread\0dataReceived\0\0position\0"
    "velocity\0torque\0statusUpdated\0MotorStatus\0"
    "status\0errorSignal\0error\0messageReceived\0"
    "msg\0motorStateChanged\0newDataReceived\0"
    "motorStatusUpdated\0errorOccurred"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EtherCATThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   54,    2, 0x06 /* Public */,
       6,    1,   61,    2, 0x06 /* Public */,
       9,    1,   64,    2, 0x06 /* Public */,
      11,    1,   67,    2, 0x06 /* Public */,
      13,    0,   70,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    3,   71,    2, 0x0a /* Public */,
      15,    1,   78,    2, 0x0a /* Public */,
      16,    1,   81,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Short,    3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Short,    3,    4,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void EtherCATThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EtherCATThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->dataReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< short(*)>(_a[3]))); break;
        case 1: _t->statusUpdated((*reinterpret_cast< const MotorStatus(*)>(_a[1]))); break;
        case 2: _t->errorSignal((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->messageReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->motorStateChanged(); break;
        case 5: _t->newDataReceived((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< short(*)>(_a[3]))); break;
        case 6: _t->motorStatusUpdated((*reinterpret_cast< const MotorStatus(*)>(_a[1]))); break;
        case 7: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EtherCATThread::*)(int , int , short );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EtherCATThread::dataReceived)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EtherCATThread::*)(const MotorStatus & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EtherCATThread::statusUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (EtherCATThread::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EtherCATThread::errorSignal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (EtherCATThread::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EtherCATThread::messageReceived)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (EtherCATThread::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EtherCATThread::motorStateChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EtherCATThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_EtherCATThread.data,
    qt_meta_data_EtherCATThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EtherCATThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EtherCATThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EtherCATThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int EtherCATThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void EtherCATThread::dataReceived(int _t1, int _t2, short _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EtherCATThread::statusUpdated(const MotorStatus & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EtherCATThread::errorSignal(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void EtherCATThread::messageReceived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EtherCATThread::motorStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
