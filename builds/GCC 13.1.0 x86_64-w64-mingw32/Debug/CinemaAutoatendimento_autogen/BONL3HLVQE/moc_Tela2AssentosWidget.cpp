/****************************************************************************
** Meta object code from reading C++ file 'Tela2AssentosWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.12.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../include/visual/Tela2AssentosWidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Tela2AssentosWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.12.0. It"
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
struct qt_meta_tag_ZN19Tela2AssentosWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto Tela2AssentosWidget::qt_create_metaobjectdata<qt_meta_tag_ZN19Tela2AssentosWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Tela2AssentosWidget",
        "voltarParaFilmes",
        "",
        "avancarParaBomboniere",
        "onAssentoClicado",
        "fileira",
        "coluna",
        "onVoltarClicado",
        "onAvancarClicado"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'voltarParaFilmes'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'avancarParaBomboniere'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onAssentoClicado'
        QtMocHelpers::SlotData<void(int, int)>(4, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Int, 6 },
        }}),
        // Slot 'onVoltarClicado'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAvancarClicado'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Tela2AssentosWidget, qt_meta_tag_ZN19Tela2AssentosWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Tela2AssentosWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19Tela2AssentosWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19Tela2AssentosWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN19Tela2AssentosWidgetE_t>.metaTypes,
    nullptr
} };

void Tela2AssentosWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Tela2AssentosWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->voltarParaFilmes(); break;
        case 1: _t->avancarParaBomboniere(); break;
        case 2: _t->onAssentoClicado((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->onVoltarClicado(); break;
        case 4: _t->onAvancarClicado(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Tela2AssentosWidget::*)()>(_a, &Tela2AssentosWidget::voltarParaFilmes, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Tela2AssentosWidget::*)()>(_a, &Tela2AssentosWidget::avancarParaBomboniere, 1))
            return;
    }
}

const QMetaObject *Tela2AssentosWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Tela2AssentosWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN19Tela2AssentosWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Tela2AssentosWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void Tela2AssentosWidget::voltarParaFilmes()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Tela2AssentosWidget::avancarParaBomboniere()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
