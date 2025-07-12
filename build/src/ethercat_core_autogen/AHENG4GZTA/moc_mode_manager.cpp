/****************************************************************************
** Meta object code from reading C++ file 'mode_manager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../include/qt_ui/control/mode_manager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mode_manager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ModeManager_t {
    QByteArrayData data[9];
    char stringdata0[135];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ModeManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ModeManager_t qt_meta_stringdata_ModeManager = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ModeManager"
QT_MOC_LITERAL(1, 12, 13), // "onModeChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 5), // "index"
QT_MOC_LITERAL(4, 33, 20), // "onModeConfirmClicked"
QT_MOC_LITERAL(5, 54, 19), // "onPPParamsConfirmed"
QT_MOC_LITERAL(6, 74, 19), // "onPVParamsConfirmed"
QT_MOC_LITERAL(7, 94, 19), // "onPTParamsConfirmed"
QT_MOC_LITERAL(8, 114, 20) // "onCSTParamsConfirmed"

    },
    "ModeManager\0onModeChanged\0\0index\0"
    "onModeConfirmClicked\0onPPParamsConfirmed\0"
    "onPVParamsConfirmed\0onPTParamsConfirmed\0"
    "onCSTParamsConfirmed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ModeManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x0a /* Public */,
       7,    0,   50,    2, 0x0a /* Public */,
       8,    0,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ModeManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ModeManager *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onModeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onModeConfirmClicked(); break;
        case 2: _t->onPPParamsConfirmed(); break;
        case 3: _t->onPVParamsConfirmed(); break;
        case 4: _t->onPTParamsConfirmed(); break;
        case 5: _t->onCSTParamsConfirmed(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ModeManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ModeManager.data,
    qt_meta_data_ModeManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ModeManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ModeManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ModeManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ModeManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
