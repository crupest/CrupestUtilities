#pragma once

namespace cru
{
    namespace internal_function_
    {
        template<typename Ty>
        struct InvokableObject;

        template<typename R, typename... Args>
        struct InvokableObject<R(Args...)>
        {
            virtual InvokableObject* Clone() const = 0;
            virtual R Invoke(Args&&...) = 0;
            virtual ~InvokableObject();
        };

        template<typename R, typename ...Args>
        InvokableObject<R(Args...)>::~InvokableObject()
        {

        }

        template<typename R, typename... Args>
        class FunctionPointer_ : public InvokableObject<R(Args...)>
        {
        public:
            using FunctionPointer = R(*)(Args...);

            FunctionPointer_(FunctionPointer pointer)
                : pointer_(pointer) { }

            FunctionPointer_* Clone() const
                { return new FunctionPointer_(pointer_); }
            R Invoke(Args&&... args) override
                { return (*pointer_)(std::forward<Args>(args)...); }

        private:
            FunctionPointer pointer_;
        };

        template<typename C, typename R, typename... Args>
        class MemberFunctionPointer_ : public InvokableObject<R(Args...)>
        {
        public:
            using ObjectPointer = C*;
            typedef R(C::*MemberFunctionPointer)(Args...);

            MemberFunctionPointer_(ObjectPointer object, MemberFunctionPointer pointer)
                : object_(object), pointer_(pointer) { }

            MemberFunctionPointer_* Clone() const
                { return new MemberFunctionPointer_(object_, pointer_); }
            R Invoke(Args&&... args) override
                { return (object_->*pointer_)(std::forward<Args>(args)...); }

        private:
            ObjectPointer object_;
            MemberFunctionPointer pointer_;
        };

        template<typename C, typename R, typename... Args>
        class Functor_ : public InvokableObject<R(Args...)>
        {
        public:
            using ObjectPointer = C*;

            Functor_(ObjectPointer object) : object_(object) { }

            Functor_* Clone() const { return new Functor_(object_); }
            R Invoke(Args&&... args) override
                { return (*object_)(std::forward<Args>(args)...); }

        private:
            ObjectPointer object_;
        };

        template<typename C, typename R, typename... Args>
        class Lambda_ : public InvokableObject<R(Args...)>
        {
        public:
            using LambdaType = C;

            Lambda_(const LambdaType& lambda) : lambda_(lambda) { }

            Lambda_* Clone() const { return new Lambda_(lambda_); }
            R Invoke(Args&&... args) override
                { return lambda_(std::forward<Args>(args)...); }

        public:
            LambdaType lambda_;
        };
    }


    template<typename Ty>
    class Function;

    template<typename R, typename... Args>
    class Function<R(Args...)>
    {
    public:
        Function() = default;
        Function(R(*functionPointer)(Args...))
            : function_(new internal_function_::FunctionPointer_<R, Args...>(functionPointer)) { }
        template<typename C>
        Function(C* object, R(C::*functionPointer)(Args...))
            : function_(new internal_function_::MemberFunctionPointer_<C, R, Args...>(object, functionPointer)) { }
        template<typename C>
        Function(C* object)
            : function_(new internal_function_::Functor_<C, R, Args...>(object)) { }
        template<typename Lambda>
        Function(Lambda lambda)
            : function_(new internal_function_::Lambda_<Lambda, R, Args...>(lambda)) { }

        Function(const Function& other) : function_(other.function_->Clone()) { }
        Function(Function&& other) : function_(other.function_) { other.function_ = nullptr; }
        Function& operator = (const Function& other);
        Function& operator = (Function&& other);
        ~Function();

        R operator()(Args&&... args) const
            { return function_->Invoke(std::forward<Args>(args)...); }

    private:
        void Destroy_() { if (function_) delete function_; }

        internal_function_::InvokableObject<R(Args...)>* function_ = nullptr;
    };

    template<typename R, typename ...Args>
    Function<R(Args...)> & Function<R(Args...)>::operator=(const Function & other)
    {
        if (this != &other)
        {
            Destroy_();
            function_ = other.function_->Clone();
        }
        return *this;
    }

    template<typename R, typename ...Args>
    Function<R(Args...)> & Function<R(Args...)>::operator=(Function && other)
    {
        if (this != &other)
        {
            Destroy_();
            function_ = other.function_;
            other.function_ = nullptr;
        }
        return *this;
    }

    template<typename R, typename ...Args>
    Function<R(Args...)>::~Function()
    {
        Destroy_();
    }

    template<typename C, typename R, typename... Args>
    Function<R(Args...)> make_function(C* object, R(C::*functionPointer)(Args...))
    {
        return Function<R(Args...)>(object, functionPointer);
    }
}
