/*Copyright 2016 crupest(Yang Yuqian)
 *
 *Licensed under the Apache License, Version 2.0 (the "License");
 *you may not use this file except in compliance with the License.
 *You may obtain a copy of the License at
 *
 *http ://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing, software
 *distributed under the License is distributed on an "AS IS" BASIS,
 *WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *See the License for the specific language governing permissions and
 *limitations under the License.
 */


//A frame for SI(The International System of Units) written with c++ templates.
//Developer : crupest


#pragma once

#include <cmath>

namespace unit
{
	namespace internal
	{

////////////////////////////////////////////////////////////////////////////////////////////////////
//		Global type for the whole file
////////////////////////////////////////////////////////////////////////////////////////////////////

		//define the integer type.
		using IntegerType = int;

		//define the float type
		using FloatType = double;

		//define the type to store value of the quantity in class Unit.
		using NumericType = double;
		constexpr NumericType numericZero = 0.0;
		constexpr NumericType numericTen = 10.0;

		//define the type to store value of multiple factor.
		using MultipleFactorValueType = FloatType;
		constexpr MultipleFactorValueType defaultMultipleFactorValue = 1.0;

		//macro to define a class to wrap float
		//no ';' in the end
		//Remember that class name can't be identical
		//explanation:
		//Because C++ doesn't support float type as the template argument,
		//use this wrapper to convey float value as a type,
		//and get it by static member "value".
		#define Define_FloatWrapperType(theValue, className)							\
		struct className																\
		{																				\
			constexpr static unit::internal::FloatType value = theValue;				\
		};

		//macro to define a class to wrap multiple factor value
		//no ';' in the end
		//Remember that class name can't be identical
		#define Define_MultipleFactorValueWrapperType(theValue, className)				\
		struct className																\
		{																				\
			constexpr static unit::internal::MultipleFactorValueType value = theValue;	\
		};

		//macro to define a multiple factor
		//no ';' in the end
		//Remember that class name can't be identical
		#define Define_MultipleFactorType(multipleFactor, className) \
			Define_MultipleFactorValueWrapperType(multipleFactor, className)

		//multiple factor is 1.
		Define_MultipleFactorType(defaultMultipleFactorValue, DefaultMultipleFactorType)

////////////////////////////////////////////////////////////////////////////////////////////////////
//		Classes definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

		//chart for SI.
		//dimension_symbol	quantity_name				unit_symbol
		//L					length						m
		//M					mass						kg
		//T					time						s
		//I					electric_current			A
		//Θ(P instead!)		thermodynamic_temperature	K
		//N					amount of substance			mol
		//J					luminous_intensity			cd

		//class of dimension.
		//all dimensions are set with 0 in default.
		template<IntegerType _L = 0, IntegerType _M = 0, IntegerType _T = 0, IntegerType _I = 0, IntegerType _P = 0, IntegerType _N = 0, IntegerType _J = 0>
		struct Dimension
		{
			constexpr static IntegerType L = _L;
			constexpr static IntegerType M = _M;
			constexpr static IntegerType T = _T;
			constexpr static IntegerType I = _I;
			constexpr static IntegerType P = _P;
			constexpr static IntegerType N = _N;
			constexpr static IntegerType J = _J;
		};

		//NoDimension
		using NoDimension = Dimension<0, 0, 0, 0, 0, 0, 0>;

		//class of unit
		template<typename _Dimension, typename _MultipleFactorType = DefaultMultipleFactorType>
		struct Unit
		{
			NumericType value = numericZero;

			typedef _Dimension Dimension;
			typedef _MultipleFactorType MultipleFactorType;
			constexpr static MultipleFactorValueType multipleFactor = MultipleFactorType::value;

			Unit() = default;

			explicit Unit(NumericType _value) : value(_value) { }

			Unit(const Unit&) = default;
			Unit& operator = (const Unit&) = default;

			Unit operator - () const { return Unit(-value); }
			template<typename _OtherMultipleFactorType>
			Unit& operator +=(const Unit<Dimension, _OtherMultipleFactorType>& other);
			template<typename _OtherMultipleFactorType>
			Unit& operator -=(const Unit<Dimension, _OtherMultipleFactorType>& other);
			Unit& operator *=(NumericType factor);
			Unit& operator /=(NumericType factor);

			////////////////////////////////////////////////
			//type conversion operators

			//sometimes extremely dangerous!!!
			explicit operator NumericType() const { return value; }

			explicit operator bool() const { return value; }

			template<typename _NewMultipleFactorType>
			operator Unit<Dimension, _NewMultipleFactorType>() const { return Unit<Dimension, _NewMultipleFactorType>(value * multipleFactor / _NewMultipleFactorType::value); }
		
		};

		
		//when no dimension,
		//it acts like a NumericType.
		//just like autoboxing and autounboxing.
		template<typename _MultipleFactorType>
		struct Unit<NoDimension, _MultipleFactorType>
		{
			NumericType value = numericZero;

			typedef NoDimension Dimension;
			typedef _MultipleFactorType MultipleFactorType;
			constexpr static MultipleFactorValueType multipleFactor = MultipleFactorType::value;

			Unit() = default;

			Unit(NumericType _value) : value(_value) { }

			Unit(const Unit&) = default;
			Unit& operator = (const Unit&) = default;

			Unit operator - () const { return Unit(-value); }
			template<typename _OtherMultipleFactorType>
			Unit& operator +=(const Unit<Dimension, _OtherMultipleFactorType>& other);
			template<typename _OtherMultipleFactorType>
			Unit& operator -=(const Unit<Dimension, _OtherMultipleFactorType>& other);
			Unit& operator *=(NumericType factor);
			Unit& operator /=(NumericType factor);

			////////////////////////////////////////////////
			//type conversion operators

			operator NumericType() const { return value; }

			explicit operator bool() const { return value; }

			template<typename _NewMultipleFactorType>
			operator Unit<Dimension, _NewMultipleFactorType>() const { return Unit<Dimension, _NewMultipleFactorType>(value * multipleFactor / _NewMultipleFactorType::value); }

		};


////////////////////////////////////////////////////////////////////////////////////////////////////
//						Type Calculation Regulations
//1.If a calculator type should return a bool,
//	the "static const bool" member "resultValue" would be the bool value result,
//	and the "ResultType" would be the type result, indicated with YesType or NoType.
//2.If a calculator type should only return an IntegerType value,
//	the result would be "static const IntegerType resultValue".
//3.If a calculator type should only return a type,
//	the result would be "ResultType".
////////////////////////////////////////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////////////
		//convert bool to type
		#define GetTypeFromBool(boolean) typename BoolToType<boolean>::ResultType

		//In a class for checking, define resultValue by ResultType.
		//remember ";"!
		#define DEFINERESULTVALUE static const bool resultValue = ResultType::value

		//In a class for checking, define ResultType by resultValue.
		//remember ";"!
		#define DEFINERESULTTYPE typedef GetTypeFromBool(resultValue) ResultType

		////////////////////////////////////////////////////////////////////////
		struct YesType
		{
			static const bool value = true;
		};

		struct NoType
		{
			static const bool value = false;
		};


		////////////////////////////////////////////////////////////////////////
		//convert a bool value into Yes/No type.
		template<bool argument>
		struct BoolToType
		{
			typedef NoType ResultType;
		};

		template<>
		struct BoolToType<true>
		{
			typedef YesType ResultType;
		};


		////////////////////////////////////////////////////////////////////////
		//check whether two types are the same
		template<typename LeftType, typename RightType>
		struct IsTypeSame
		{
			typedef NoType ResultType;
			DEFINERESULTVALUE;
		};

		template<typename TheSameType>
		struct IsTypeSame<TheSameType, TheSameType>
		{
			typedef YesType ResultType;
			DEFINERESULTVALUE;
		};


		////////////////////////////////////////////////////////////////////////
		//implement "if(...) ... else ..." with Type
		//if condition is true, return left, otherwise, right
		template<typename LeftType, typename RightType, bool condition>
		struct TypeConditionStatement
		{
			typedef RightType ResultType;
		};

		template<typename LeftType, typename RightType>
		struct TypeConditionStatement<LeftType, RightType, true>
		{
			typedef LeftType ResultType;
		};


		////////////////////////////////////////////////////////////////////////
		//check whether two Units have the same Dimension
		template<typename LeftUnit, typename RightUnit>
		struct IsDimensionSame
		{
			typedef typename IsTypeSame<typename LeftUnit::Dimension, typename RightUnit::Dimension>::ResultType ResultType;
			DEFINERESULTVALUE;
		};


		////////////////////////////////////////////////////////////////////////
		//check whether two Units have the same multiple factor
		template<typename LeftUnit, typename RightUnit>
		struct IsMultipleFactorSame
		{
			static const bool resultValue = LeftUnit::multipleFactor == RightUnit::multipleFactor;
			DEFINERESULTTYPE;
		};

		
		////////////////////////////////////////////////////////////////////////
		constexpr MultipleFactorValueType _getBetterMultipleFactorTypeHelper1(MultipleFactorValueType argument)
		{
			return argument >= 1 ? argument : DefaultMultipleFactorType::value / argument;
		}

		constexpr bool _getBetterMultipleFactorTypeHelper2(MultipleFactorValueType left, MultipleFactorValueType right)
		{
			return _getBetterMultipleFactorTypeHelper1(left) <= _getBetterMultipleFactorTypeHelper1(right);
		}

		template<typename LeftMultipleFactorType, typename RightMultipleFactorType>
		struct GetBetterMultipleFactorType
		{
			typedef typename TypeConditionStatement<LeftMultipleFactorType, RightMultipleFactorType, _getBetterMultipleFactorTypeHelper2(LeftMultipleFactorType::value, RightMultipleFactorType::value)>::ResultType ResultType;
		};


		////////////////////////////////////////////////////////////////////////
		//------------------------------
		//get the result type of multiplying two Dimensions.
		template<typename LeftDimension, typename RightDimension>
		using DimensionMultiplyResultType =
			Dimension<LeftDimension::L + RightDimension::L,
			LeftDimension::M + RightDimension::M,
			LeftDimension::T + RightDimension::T,
			LeftDimension::I + RightDimension::I,
			LeftDimension::P + RightDimension::P,
			LeftDimension::N + RightDimension::N,
			LeftDimension::J + RightDimension::J>;

		//------------------------------
		//get the result type of dividing two Dimensions.
		template<typename LeftDimension, typename RightDimension>
		using DimensionDivideResultType =
			Dimension<LeftDimension::L - RightDimension::L,
			LeftDimension::M - RightDimension::M,
			LeftDimension::T - RightDimension::T,
			LeftDimension::I - RightDimension::I,
			LeftDimension::P - RightDimension::P,
			LeftDimension::N - RightDimension::N,
			LeftDimension::J - RightDimension::J>;



////////////////////////////////////////////////////////////////////////////////////////////////////
//		Operators of Unit
////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline auto operator + (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			using ResultType = Unit<_Dimension, typename GetBetterMultipleFactorType<_LeftMultipleFactorType,_RightMultipleFactorType>::ResultType>;
			return ResultType(static_cast<ResultType>(left).value + static_cast<ResultType>(right).value);
		}
		
		
		////////////////////////////////////////////////////
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline auto operator - (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			using ResultType = Unit<_Dimension, typename GetBetterMultipleFactorType<_LeftMultipleFactorType, _RightMultipleFactorType>::ResultType>;
			return ResultType(static_cast<ResultType>(left).value - static_cast<ResultType>(right).value);
		}


		////////////////////////////////////////////////////
		template<typename _LDimension, typename _RDimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline auto operator * (const Unit<_LDimension, _LeftMultipleFactorType>& left, const Unit<_RDimension, _RightMultipleFactorType>& right)
		{
			using ResultMutilpleFactorType = typename GetBetterMultipleFactorType<_LeftMultipleFactorType, _RightMultipleFactorType>::ResultType;
			using ResultType = Unit<DimensionMultiplyResultType<_LDimension, _RDimension>, ResultMutilpleFactorType>;
			return ResultType(static_cast<Unit<_LDimension, ResultMutilpleFactorType>>(left).value * static_cast<Unit<_RDimension, ResultMutilpleFactorType>>(right).value);
		}

		template<typename _Dimension, typename _MultipleTypeDimension>
		inline auto operator * (const Unit<_Dimension, _MultipleTypeDimension>& left, NumericType right)
		{
			return Unit<_Dimension, _MultipleTypeDimension>(left.value * right);
		}

		template<typename _Dimension, typename _MultipleTypeDimension>
		inline auto operator * (NumericType left, const Unit<_Dimension, _MultipleTypeDimension>& right)
		{
			return Unit<_Dimension, _MultipleTypeDimension>(right.value * left);
		}


		////////////////////////////////////////////////////
		template<typename _LDimension, typename _RDimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline auto operator / (const Unit<_LDimension, _LeftMultipleFactorType>& left, const Unit<_RDimension, _RightMultipleFactorType>& right)
		{
			using ResultMutilpleFactorType = typename GetBetterMultipleFactorType<_LeftMultipleFactorType, _RightMultipleFactorType>::ResultType;
			using ResultType = Unit<DimensionDivideResultType<_LDimension, _RDimension>, ResultMutilpleFactorType>;
			return ResultType(static_cast<Unit<_LDimension, ResultMutilpleFactorType>>(left).value / static_cast<Unit<_RDimension, ResultMutilpleFactorType>>(right).value);
		}
		
		template<typename _Dimension, typename _MultipleTypeDimension>
		inline auto operator / (const Unit<_Dimension, _MultipleTypeDimension>& left, NumericType right)
		{
			return Unit<_Dimension, _MultipleTypeDimension>(left.value / right);
		}


		////////////////////////////////////////////////////
		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType _compare(const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			using ResultType = Unit<_Dimension, typename GetBetterMultipleFactorType<_LeftMultipleFactorType, _RightMultipleFactorType>::ResultType>;
			return static_cast<ResultType>(left).value - static_cast<ResultType>(right).value;
		}

		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator == (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) == numericZero;
		}


		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator != (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) != numericZero;
		}

		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator >= (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) >= numericZero;
		}

		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator <= (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) <= numericZero;
		}

		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator > (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) > numericZero;
		}

		//------------------------------
		template<typename _Dimension, typename _LeftMultipleFactorType, typename _RightMultipleFactorType>
		inline NumericType operator < (const Unit<_Dimension, _LeftMultipleFactorType>& left, const Unit<_Dimension, _RightMultipleFactorType>& right)
		{
			return _compare(left, right) < numericZero;
		}


		////////////////////////////////////////////////////
		//------------------------------
		template<typename _Dimension, typename _MultipleFactorType>
		template<typename _OtherMultipleFactorType>
		inline Unit<_Dimension, _MultipleFactorType>& Unit<_Dimension, _MultipleFactorType>::operator+=(const Unit<Dimension, _OtherMultipleFactorType>& other)
		{
			value += static_cast<Unit<_Dimension, _MultipleFactorType>>(other).value;
			return *this;
		}

		//------------------------------
		template<typename _Dimension, typename _MultipleFactorType>
		template<typename _OtherMultipleFactorType>
		inline Unit<_Dimension, _MultipleFactorType>& Unit<_Dimension, _MultipleFactorType>::operator-=(const Unit<Dimension, _OtherMultipleFactorType>& other)
		{
			value -= static_cast<Unit<_Dimension, _MultipleFactorType>>(other).value;
			return *this;
		}

		//------------------------------
		template<typename _Dimension, typename _MultipleFactorType>
		inline Unit<_Dimension, _MultipleFactorType> & Unit<_Dimension, _MultipleFactorType>::operator*=(NumericType factor)
		{
			value *= factor;
			return *this;
		}
		
		//------------------------------
		template<typename _Dimension, typename _MultipleFactorType>
		inline Unit<_Dimension, _MultipleFactorType> & Unit<_Dimension, _MultipleFactorType>::operator/=(NumericType factor)
		{
			value /= factor;
			return *this;
		}

	}//close namespace "internal"



////////////////////////////////////////////////////////////////////////////////////////////////////
//		Functions of Unit
////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename _Dimension, typename _MultipleFactorType>
	auto sqrt(const internal::Unit<_Dimension, _MultipleFactorType>& quantity)
	{
		return internal::Unit<internal::Dimension
			<_Dimension::L/2,
			_Dimension::M/2,
			_Dimension::T/2,
			_Dimension::I/2,
			_Dimension::P/2,
			_Dimension::N/2,
			_Dimension::J/2>,
			_MultipleFactorType>
			(std::sqrt(quantity.value));
	}

////////////////////////////////////////////////////////////////////////////////////////////////////
//		Concrete multipleFactorTypes
////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace multipleFactorType
	{
		Define_MultipleFactorType(1.0e+24, prefix_Y);
		Define_MultipleFactorType(1.0e+21, prefix_Z);
		Define_MultipleFactorType(1.0e+18, prefix_E);
		Define_MultipleFactorType(1.0e+15, prefix_P);
		Define_MultipleFactorType(1.0e+12, prefix_T);
		Define_MultipleFactorType(1.0e+9, prefix_G);
		Define_MultipleFactorType(1.0e+6, prefix_M);
		Define_MultipleFactorType(1.0e+3, prefix_k);
		Define_MultipleFactorType(1.0e+2, prefix_h);
		Define_MultipleFactorType(1.0e+1, prefix_da);
		Define_MultipleFactorType(1.0e-1, prefix_d);
		Define_MultipleFactorType(1.0e-2, prefix_c);
		Define_MultipleFactorType(1.0e-3, prefix_m);
		Define_MultipleFactorType(1.0e-6, prefix_w); // use w instead of micro
		Define_MultipleFactorType(1.0e-9, prefix_n);
		Define_MultipleFactorType(1.0e-12, prefix_p);
		Define_MultipleFactorType(1.0e-15, prefix_f);
		Define_MultipleFactorType(1.0e-18, prefix_a);
		Define_MultipleFactorType(1.0e-21, prefix_z);
		Define_MultipleFactorType(1.0e-24, prefix_y);

		Define_MultipleFactorType(60, _MiniteToSecond);
		Define_MultipleFactorType(3600, _HourToSecond);
		Define_MultipleFactorType(3600 * 24, _DayToSecond);

		Define_MultipleFactorType(3.6e6, _kw_hToJ);
	}



////////////////////////////////////////////////////////////////////////////////////////////////////
//		Concrete Units
////////////////////////////////////////////////////////////////////////////////////////////////////

	inline namespace concreteUnit
	{
		using m = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>>;
		using km = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_k>;
		using dm = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_d>;
		using cm = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_c>;
		using mm = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_m>;
		using wm = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_w>;
		using nm = internal::Unit<internal::Dimension<1, 0, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_n>;


		using kg = internal::Unit<internal::Dimension<0, 1, 0, 0, 0, 0, 0>>;
		using t = internal::Unit<internal::Dimension<0, 1, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_k>;
		using g = internal::Unit<internal::Dimension<0, 1, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_m>;
		using mg = internal::Unit<internal::Dimension<0, 1, 0, 0, 0, 0, 0>, unit::multipleFactorType::prefix_w>;

		using s = internal::Unit<internal::Dimension<0, 0, 1, 0, 0, 0, 0>>;
		using min = internal::Unit<internal::Dimension<0, 0, 1, 0, 0, 0, 0>, unit::multipleFactorType::_MiniteToSecond>;
		using h = internal::Unit<internal::Dimension<0, 0, 1, 0, 0, 0, 0>, unit::multipleFactorType::_HourToSecond>;
		using day = internal::Unit<internal::Dimension<0, 0, 1, 0, 0, 0, 0>, unit::multipleFactorType::_DayToSecond>;
		using ms = internal::Unit<internal::Dimension<0, 0, 1, 0, 0, 0, 0>, unit::multipleFactorType::prefix_m>;

		using m2 = internal::Unit<internal::Dimension<2, 0, 0, 0, 0, 0, 0>>;

		using m3 = internal::Unit<internal::Dimension<3, 0, 0, 0, 0, 0, 0>>;

		using kg_pm3 = internal::Unit<internal::Dimension<-3, 1, 0, 0, 0, 0, 0>>;

		using m_ps = internal::Unit<internal::Dimension<1, 0, -1, 0, 0, 0, 0>>;

		using m_ps2 = internal::Unit<internal::Dimension<1, 0, -2, 0, 0, 0, 0>>;

		using N = internal::Unit<internal::Dimension<1, 1, -2, 0, 0, 0, 0>>;

		using J = internal::Unit<internal::Dimension<2, 1, -2, 0, 0, 0, 0>>;
		using kW_h = internal::Unit<internal::Dimension<2, 1, -2, 0, 0, 0, 0>, unit::multipleFactorType::_kw_hToJ>;

		using W = internal::Unit<internal::Dimension<2, 1, -3, 0, 0, 0, 0>>;

		using Pa = internal::Unit<internal::Dimension<-1, 1, -2, 0, 0, 0, 0>>;

		using A = internal::Unit<internal::Dimension<0, 0, 0, 1, 0, 0, 0>>;

		using C = internal::Unit<internal::Dimension<0, 0, 1, 1, 0, 0, 0>>;

		using V = internal::Unit<internal::Dimension<2, 1, -3, -1, 0, 0, 0>>;

		using V_pm = internal::Unit<internal::Dimension<1, 1, -3, -1, 0, 0, 0>>;
		using N_pC = V_pm;

		using T = internal::Unit<internal::Dimension<0, 1, -2, -1, 0, 0, 0>>;

		using Wb = internal::Unit<internal::Dimension<2, 1, -2, -1, 0, 0, 0>>;

		using K = internal::Unit<internal::Dimension<0, 0, 0, 0, 1, 0, 0>>;
	}
}
