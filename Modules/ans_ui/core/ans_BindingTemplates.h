/**
 Experimental UI Framework
 -------------------------
 Copyright 2019 me-ans@GitHub
 
 This is incomplete work in progress and mainly for proof-of-concept, education
 and discussion. Please regard the license terms accompanying this Juce module.
 */

#pragma once

//#include "ans_Binding.h"
//#include "../adaptors/ans_adaptor_base.h"

namespace ans
{
    using namespace juce;
    
/**
 This struct is used to deduce from a setter Binding's member function argument type the kind of
 type that works best as a temporary buffer passed to adaptor.getComponentValue()
 */
namespace TypeHandlers
{

    /** Strip any qualifiers */
    template <typename Type> struct BaseType                    { using type = Type; };
    template <typename Type> struct BaseType <Type&>            { using type = Type; };
    template <typename Type> struct BaseType <Type*>            { using type = Type; };
    template <typename Type> struct BaseType <const Type>       { using type = Type; };
    template <typename Type> struct BaseType <const Type&>      { using type = Type; };
    template <typename Type> struct BaseType <const Type*>      { using type = Type; };
    
    /** Value types that can be passsed directly to a compiler-resolved version of setComponentValue() */
    template <typename Type> struct PassDirectly                   { enum { value = false }; };
    template <>              struct PassDirectly <String>          { enum { value = true }; };
    template <>              struct PassDirectly <const String>    { enum { value = true }; };
    template <>              struct PassDirectly <const char*>     { enum { value = true }; };
    template <>              struct PassDirectly <const wchar_t*>  { enum { value = true }; };
    template <>              struct PassDirectly <bool>            { enum { value = true }; };
    template <>              struct PassDirectly <int>             { enum { value = true }; };
    template <>              struct PassDirectly <double>          { enum { value = true }; };
    
    /**
     Value types that get delegated to the var version of UIAdaptor::setComponentValue() immediately.
     Note: All types compatible with var will eventually be delegated there too, unless the adaptor overrides that specific type.
     */
    template <typename Type> struct UseVariant                  { enum { value = false }; };
    template <>              struct UseVariant <var>            { enum { value = true }; };
    template <>              struct UseVariant <const var>      { enum { value = true }; };
    template <>              struct UseVariant <int64>          { enum { value = true }; };
    
    //-----------------------------------------------------------------------------------------------
    
    template <typename T>
    constexpr int getter_argument_mode()
    {
        if (PassDirectly<T>::value)         return 0;
        if (UseVariant<T>::value)           return 2;
        if (std::is_pointer<T>::value)      return 0;
        if (std::is_reference<T>::value)    return 0;
        if (std::is_object<T>::value)       return 1;
        return -1;
    }
    
    /** Default Case: Unsupported */
    template <typename ReturnType, int Mode>
    struct Getter
    {
        template <typename ModelClass, typename MEMBER>
        Getter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            DBG ("*** ERROR: Getter binding does not support return type of " << source);
            jassertfalse;
        }
    };
    
    /** Case 0: Pass return type of model function immediately to setComponentState() */
    template <typename ReturnType>
    struct Getter<ReturnType, 0>
    {
        template <typename ModelClass, typename MEMBER>
        Getter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            adaptor.setComponentState (p, MEMBER_FN (model, member)());
        }
    };

    /** Case 1: Copy-by-value objects provided by model that get passed as a reference to setComponentState() */
    template <typename ReturnType>
    struct Getter<ReturnType, 1>
    {
        template <typename ModelClass, typename MEMBER>
        Getter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            ReturnType buffer (MEMBER_FN (model, member)()); // performance hit for large objects
            adaptor.setComponentState (p, buffer);
        }
    };
    
    /** Case 2: Use Juce variant type as an intermediate vessel */
    template <typename ReturnType>
    struct Getter<ReturnType, 2>
    {
        template <typename ModelClass, typename MEMBER>
        Getter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            var buffer (MEMBER_FN (model, member)());
            adaptor.setComponentState (p, buffer); // should use var& as argument
        }
    };

    
    //-----------------------------------------------------------------------------------------------
    
    template <typename T>
    constexpr int setter_argument_mode()
    {
        if (std::is_arithmetic<T>::value)       return 1;
        if (std::is_pointer<T>::value)          return 2;
        if (std::is_reference<T>::value)        return 3;
        if (std::is_trivial<T>::value)          return 0;
        return -1;
    }
    
    /** Default Case: Unsupported */
    template <typename ArgumentType, int Mode>
    struct Setter
    {
        template <typename ModelClass, typename MEMBER>
        Setter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            DBG ("*** ERROR: Setter binding does not support argument type of " << source);
            jassertfalse;
        }
    };

    /** Case 0: Copy-by-value non-primitive data */
    template <typename ArgumentType>
    struct Setter<ArgumentType, 0>
    {
        template <typename ModelClass, typename MEMBER>
        Setter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            using BaseType = typename TypeHandlers::BaseType<ArgumentType>::type;
            BaseType value; // performance hit for large objects
            BaseType& buffer = value;
            adaptor.getComponentState (p, buffer);
            MEMBER_FN (model, member)(value);
        }
    };
    
    /** Case 1: Arithmetic values that default to zero */
    template <typename ArgumentType>
    struct Setter<ArgumentType, 1>
    {
        template <typename ModelClass, typename MEMBER>
        Setter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            using BaseType = typename TypeHandlers::BaseType<ArgumentType>::type;
            BaseType value = 0;
            BaseType& buffer = value;
            adaptor.getComponentState (p, buffer);
            MEMBER_FN (model, member)(value);
        }
    };
    
    /** Case 2: Pass by pointer, e.g. setupComponent (Component* comp) */
    template <typename ArgumentType>
    struct Setter<ArgumentType, 2>
    {
        template <typename ModelClass, typename MEMBER>
        Setter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            ArgumentType pointer = nullptr;
            ArgumentType& buffer = pointer;
            adaptor.getComponentState (p, buffer);
            MEMBER_FN (model, member)(pointer);
        }
    };
    
    /** Case 3: Pass by reference, e.g. setListSelection (Selection& selection)  */
    template <typename ArgumentType>
    struct Setter<ArgumentType, 3>
    {
        template <typename ModelClass, typename MEMBER>
        Setter (ModelClass* model, UIAdaptor& adaptor, const Binding::Purpose& p, MEMBER& member, const String& source)
        {
            using BaseType = typename TypeHandlers::BaseType<ArgumentType>::type;
            BaseType value; // performance hit for large objects
            BaseType& buffer = value;
            adaptor.getComponentState (p, buffer);
            MEMBER_FN (model, member)(value);
        }
    };
    
}


/**
 Binding for getting a value from UIModel and providing it to the Component:
 Model -> Component
 */
template <typename ModelClass, typename ReturnType>
class GetterBinding : public Binding
{
public:
    typedef ReturnType (ModelClass::*Method)();
    
    GetterBinding (ModelClass* receiver,
                   ReturnType (ModelClass::*method)(),
                   const String& source,
                   const Binding::Purpose& p,
                   Aspect a = Model::Undefined) :
    
        Binding (p, source, a),
        model (receiver),
        member (method)
    {
        jassert (purpose.signature == Binding::Signature::Update_Component);
    }
    
    bool isBound() const override { return model != nullptr; }
    
    Binding* instantiateFor (UIModel* instance) override
    {
        auto receiver = dynamic_cast<ModelClass*>(instance);
        /** If you get an assertion here, your binding is likely bound to a different class of UIModel! */
        jassert (receiver != nullptr);
        return new GetterBinding (receiver, member, source, purpose, aspect);
    }
    
    void performFor (UIAdaptor& adaptor) override
    {
        jassert (isBound());
        TypeHandlers::Getter<ReturnType, TypeHandlers::getter_argument_mode<ReturnType>()> (model, adaptor, purpose, member, source);
    }
    
private:
    ModelClass* model;
    Method member;
};



/**
 Binding for getting a value from the Component and providing it to UIModel:
 Model <- Component
 */
template <typename ModelClass, typename ArgumentType>
class SetterBinding : public Binding
{
public:
    typedef void (ModelClass::*Method)(ArgumentType);
    
    SetterBinding (ModelClass* receiver,
                   void (ModelClass::*method)(ArgumentType),
                   const String& source,
                   const Binding::Purpose& p) :
        Binding (p, source),
        model (receiver),
        member (method)
    {
        jassert (purpose.signature == Binding::Signature::Update_Model);
    }
    
    bool isAspected() const override { return false; }
    
    bool isBound() const override { return model != nullptr; }
    
    Binding* instantiateFor (UIModel* instance) override
    {
        auto receiver = dynamic_cast<ModelClass*>(instance);
        jassert (receiver != nullptr);
        return new SetterBinding (receiver, member, source, purpose);
    }
    
    void performFor (UIAdaptor& adaptor) override
    {
        jassert (isBound());
        TypeHandlers::Setter<ArgumentType, TypeHandlers::setter_argument_mode<ArgumentType>()> (model, adaptor, purpose, member, source);
    }

private:
    ModelClass* model;
    Method member;
};




/**
 Binding for UIModel to perform an action with no arguments.
 */
template <typename ModelClass>
class ActionBinding : public Binding
{
public:
    typedef void (ModelClass::*Method)();
    
    ActionBinding (ModelClass* receiver,
                   void (ModelClass::*method)(),
                   const String& source,
                   const Binding::Purpose& p) :
        Binding (p, source),
        model (receiver),
        member (method)
    {
        jassert (purpose.signature == Binding::Signature::Trigger_Action);
    }
    
    bool isAspected() const override { return false; }
    
    bool isBound() const override { return model != nullptr; }
    
    Binding* instantiateFor (UIModel* instance) override
    {
        auto receiver = dynamic_cast<ModelClass*>(instance);
        /** If you get an assertion here, your binding is likely bound to a different class of UIModel! */
        jassert (receiver != nullptr);
        return new ActionBinding (receiver, member, source, purpose);
    }
    
    void performFor (UIAdaptor& adaptor) override
    {
        jassert (isBound());
        MEMBER_FN (model, member)();
    }
    
private:
    ModelClass* model;
    Method member;
};





/**
 Binding that passes the Component to UIModel, so it can configure it in any way it chooses.
 Model <- Component
 */
template <typename ModelClass>
class ConfigBinding : public Binding
{
public:
    typedef void (ModelClass::*Method)(Component*);
    
    ConfigBinding (ModelClass* receiver,
                   void (ModelClass::*method)(Component*),
                   const String& source,
                   const Binding::Purpose& p,
                   Aspect a = Model::Undefined) :
        Binding (p, source, a),
        model (receiver),
        member (method)
    {
        jassert (purpose.signature == Binding::Signature::Pass_Component);
    }
    
    bool isBound() const override { return model != nullptr; }
    
    Binding* instantiateFor (UIModel* instance) override
    {
        auto receiver = dynamic_cast<ModelClass*>(instance);
        /** If you get an assertion here, your binding is likely bound to a different class of UIModel! */
        jassert (receiver != nullptr);
        return new ConfigBinding (receiver, member, source, purpose, aspect);
    }
    
    void performFor (UIAdaptor& adaptor) override
    {
        jassert (isBound());
        MEMBER_FN (model, member)(adaptor.getComponent());
    }
    
private:
    ModelClass* model;
    Method member;
};




/**
 Binding that asks UIModel to (re)populate an initially empty UIComposite
 */
template <typename ModelClass>
class CanvasBinding : public Binding
{
public:
    typedef void (ModelClass::*Method)(UIComposite*);
    
    CanvasBinding (ModelClass* receiver,
                   void (ModelClass::*method)(UIComposite*),
                   const String& source,
                   const Binding::Purpose& p,
                   Aspect a = Model::Undefined) :
        Binding (p, source, a),
        model (receiver),
        member (method)
    {
        jassert (purpose.signature == Binding::Signature::Populate_Composite);
    }
    
    bool isBound() const override { return model != nullptr; }
    
    Binding* instantiateFor (UIModel* instance) override
    {
        auto receiver = dynamic_cast<ModelClass*>(instance);
        /** If you get an assertion here, your binding is likely bound to a different class of UIModel! */
        jassert (receiver != nullptr);
        return new CanvasBinding (receiver, member, source, purpose, aspect);
    }
    
    void performFor (UIAdaptor& adaptor) override
    {
        jassert (isBound());

        if (auto comp = adaptor.getUIComposite())
        {
            adaptor.componentBuildBegin();
            MEMBER_FN (model, member)(comp);
            adaptor.componentBuildEnd();
        }
    }
    
private:
    ModelClass* model;
    Method member;
};





//=====================================================================================

/**
 Bind provides static convenience methods for creating Bindings with the least verbosity and boilerplate.
 They are initially unbound, that is, not yet associated with an instance of UIModel.
 */
struct Bind
{
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename ReturnType>
    static Binding* GetEnabled (ReturnType  (Class::*method)(),
                                const String& source,
                                Aspect aspect = Model::Undefined)
    {
        return new GetterBinding<Class,ReturnType> (nullptr, method, source, Binding::Purpose::GetEnabled, aspect);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename ReturnType>
    static Binding* GetVisible (ReturnType  (Class::*method)(),
                                const String& source,
                                Aspect aspect = Model::Undefined)
    {
        return new GetterBinding<Class,ReturnType> (nullptr, method, source, Binding::Purpose::GetVisible, aspect);
    }

    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename ReturnType>
    static Binding* GetValue (ReturnType  (Class::*method)(),
                              const String& source,
                              Aspect aspect = Model::Undefined)
    {
        return new GetterBinding<Class,ReturnType> (nullptr, method, source, Binding::Purpose::GetValue, aspect);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename ReturnType>
    static Binding* GetSelection (ReturnType  (Class::*method)(),
                                  const String& source,
                                  Aspect aspect = Model::Undefined)
    {
        return new GetterBinding<Class,ReturnType> (nullptr, method, source, Binding::Purpose::GetSelection, aspect);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename ReturnType>
    static Binding* GetLabel (ReturnType  (Class::*method)(),
                              const String& source,
                              Aspect aspect = Model::Undefined)
    {
        return new GetterBinding<Class,ReturnType> (nullptr, method, source, Binding::Purpose::GetLabel, aspect);
    }
    
    //------------------------------------------------------------------------------------
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename Value>
    static Binding* SetValue (void  (Class::*method)(Value),
                              const String& source)
    {
        return new SetterBinding<Class,Value> (nullptr, method, source, Binding::Purpose::SetValue);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename Value>
    static Binding* SetSelection (void  (Class::*method)(Value),
                                  const String& source)
    {
        return new SetterBinding<Class,Value> (nullptr, method, source, Binding::Purpose::SetSelection);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class, typename Value>
    static Binding* SetLabel (void  (Class::*method)(Value),
                              const String& source)
    {
        return new SetterBinding<Class,Value> (nullptr, method, source, Binding::Purpose::SetLabel);
    }
    
    //------------------------------------------------------------------------------------
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class>
    static Binding* Action (void  (Class::*method)(),
                            const String& source)
    {
        return new ActionBinding<Class> (nullptr, method, source, Binding::Purpose::Action);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class>
    static Binding* Config (void  (Class::*method)(Component*),
                            const String& source,
                            Aspect aspect = Model::Undefined)
    {
        return new ConfigBinding<Class> (nullptr, method, source, Binding::Purpose::Config, aspect);
    }
    
    /** Creates an initially unbound Binding, used for building UISpec */
    template <typename Class>
    static Binding* Canvas (void  (Class::*method)(UIComposite*),
                            const String& source,
                            Aspect aspect = Model::Undefined)
    {
        return new CanvasBinding<Class> (nullptr, method, source, Binding::Purpose::Canvas, aspect);
    }
    
    
};


}

