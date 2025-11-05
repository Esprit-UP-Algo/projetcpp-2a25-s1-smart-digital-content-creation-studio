/****************************************************************************
** Meta object code from reading C++ file 'gemploye.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../gemploye.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gemploye.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSGemployeENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSGemployeENDCLASS = QtMocHelpers::stringData(
    "Gemploye",
    "on_Employ_clicked",
    "",
    "on_Employ_4_clicked",
    "on_Employ_2_clicked",
    "on_Employ_6_clicked",
    "on_Employ_3_clicked",
    "on_Employ_5_clicked",
    "on_ajouter_clicked",
    "on_refrech_clicked",
    "on_modifier_clicked",
    "on_tableemp_clicked",
    "QModelIndex",
    "index",
    "on_supprimer_clicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSGemployeENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x08,    1 /* Private */,
       3,    0,   81,    2, 0x08,    2 /* Private */,
       4,    0,   82,    2, 0x08,    3 /* Private */,
       5,    0,   83,    2, 0x08,    4 /* Private */,
       6,    0,   84,    2, 0x08,    5 /* Private */,
       7,    0,   85,    2, 0x08,    6 /* Private */,
       8,    0,   86,    2, 0x08,    7 /* Private */,
       9,    0,   87,    2, 0x08,    8 /* Private */,
      10,    0,   88,    2, 0x08,    9 /* Private */,
      11,    1,   89,    2, 0x08,   10 /* Private */,
      14,    0,   92,    2, 0x08,   12 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject Gemploye::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_CLASSGemployeENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSGemployeENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSGemployeENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<Gemploye, std::true_type>,
        // method 'on_Employ_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Employ_4_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Employ_2_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Employ_6_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Employ_3_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_Employ_5_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_ajouter_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_refrech_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_modifier_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_tableemp_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'on_supprimer_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void Gemploye::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Gemploye *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_Employ_clicked(); break;
        case 1: _t->on_Employ_4_clicked(); break;
        case 2: _t->on_Employ_2_clicked(); break;
        case 3: _t->on_Employ_6_clicked(); break;
        case 4: _t->on_Employ_3_clicked(); break;
        case 5: _t->on_Employ_5_clicked(); break;
        case 6: _t->on_ajouter_clicked(); break;
        case 8: _t->on_modifier_clicked(); break;
        case 9: _t->on_tableemp_clicked((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 10: _t->on_supprimer_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject *Gemploye::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Gemploye::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSGemployeENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Gemploye::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
