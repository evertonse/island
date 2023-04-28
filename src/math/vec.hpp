#pragma once
#include <array>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include "utils/common.h"
#include "math/useful.hpp"

namespace cyx {

	template <class T, u32 DIM>
	struct vec {
	
	public:
		std::array<T,DIM> numbers;
	
        using vec_concrete = vec<T,DIM>;
	public:
		vec() {}
		vec(std::initializer_list<T> values){
			size_t index = 0;
			for (auto& val : values) {
				numbers[index++] = val;
			}
		}
        
        vec(T *vals) {
           for(int i = 0; i < DIM; i++){
                numbers[i] = vals[i];
           } 
        }


        vec(const T *vals) {
           for(int i = 0; i < DIM; i++){
                numbers[i] = vals[i];
           } 
        }


        template <typename... Args>
        vec(Args... args) : numbers{{args...}} {
            static_assert(sizeof...(args) == DIM, "Number of arguments does not match vector dimension.");
        }

		T& operator[]  (i32 index) { return numbers[index]; }
		const T& operator[]  (i32 index) const { return numbers[index]; }
		T& operator()  (i32 index) { return numbers[index]; }
		const T& operator()  (i32 index) const { return numbers[index]; }
        vec_concrete& operator=(const vec_concrete& other) {
            size_t count = this->numbers.size();
            for (size_t i = 0; i < count; i++) {
               (*this)[i] = other[i];
            }
           return *this; 
        }
        // Copy assignment operator
        //vec& operator=(const vec& other) { if (this != &other) { numbers = other.numbers; } return *this; }
		//vec operator=(const vec& v) = default;

		vec(const vec<T, DIM>& other) : numbers(other.numbers) {};
        vec(vec<T, DIM>&& other) : numbers(std::move(other.numbers)) {}


		/*==== vec operations ====*/
		f32 norm() const { return (f32) std::sqrt(norm2()) ; }
		
		f32 norm2() const { 
			// just in case we donnst want a f32 here and instead use T, we'll leave the 
			// startic casting
			f32 acc = 0.0f;
			for (auto& n : numbers) {
				acc += (n*n);
			}
			return static_cast<f32>(acc); 
		}

		static vec<f32,DIM>& normalize(const vec<f32,DIM>& self) { 
			f32 scale = 1.0 / norm(); 
			for (size_t i = 0; i < DIM; i++) {
				self[i] = static_cast<f32>(self[i]) * scale; 
            }
			return self;
		}

		vec<f32,DIM> normalize() const { 
			vec<f32,DIM> v;
			f32 scale = 1.0 / norm(); 
			for (size_t i = 0; i < DIM; i++) {
				v[i] = static_cast<f32>((*this)[i]) * scale; }
			return v;
		}

		T dot(const vec& other) const {
			auto acc = static_cast<T>(0);

			for (size_t i = 0; i < DIM; i++) {
				acc += other[i] * (*this)[i];
			}
			return acc; 
		}

        // dot product operator
        static T dot(const vec& v1, const vec& v2) {
            T result = 0;
            for (u32 i = 0; i < DIM; i++) {
                result += v1[i] * v2[i];
            }
            return result;
        }


		/*==== Operator Overloading for other vec ====*/
		vec  operator +  (const vec& other) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = other[i] + (*this)[i];
			}
			return new_vec;
		}

		vec  operator -  (const vec& other) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = other[i] - (*this)[i];
			}
			return new_vec; 
		}

		vec  operator *  (const vec& other) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = other[i] * (*this)[i];
			}
			return new_vec; 
		}

		vec&  operator +=  (const vec& other) {
            vec& self = *this;
			for (size_t i = 0; i < DIM; i++) {
				self[i] += other[i];
			}
			return self; 
        }

		vec&  operator -=  (const vec& other) {
            vec& self = *this;
			for (size_t i = 0; i < DIM; i++) {
				self[i] -= other[i];
			}
			return self; 
        }

		/*==== Operator Overloading for Integral Type ====*/
		vec  operator +  (const T& rhs) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = (*this)[i] + rhs;
			}
			return new_vec; 
		}
		vec  operator -  (const T& rhs) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = (*this)[i] - rhs;
			}
			return new_vec; 
		}

		vec  operator *  (const T& rhs) { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = (*this)[i] * rhs;
			}
			return new_vec; 
		}

		vec  operator /  (const T& rhs) const { 		
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = (*this)[i] / rhs;
			}
			return new_vec; 
		}

		vec&  operator *=  (const T& rhs) {
            vec& self = *this;
			for (size_t i = 0; i < DIM; i++) {
				self[i] *= rhs;
			}
			return self; 
        }
        
		vec&  operator /=  (const T& rhs) { 		
            vec& self = *this;
			for (size_t i = 0; i < DIM; i++) {
				self[i] /= rhs;
			}
			return *this; 
		}

		// identity operator
		vec  operator +  () const { 
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = +(*this)[i];
			}
			return new_vec; 
		}

		// negates operator
		vec  operator -  () const { 
			vec new_vec;
			for (size_t i = 0; i < DIM; i++) {
				new_vec[i] = -(*this)[i];
			}
			return new_vec; 
		}

		/*==== Operator Overloading for booleans ====*/
		bool operator==(const vec& other) {
			for (size_t i = 0; i < DIM; i++) {
				if((*this)[i] != other[i])
					return false;  
			}
			return true; 
		}
	
		bool operator != (const vec& other) const { return !this->operator==(other); }

		
		/*==== Utilities for vec ====*/
		friend std::ostream& operator << (std::ostream& os, const vec& rhs) {
			// TODO(Everton): change this to be string stream then pass to the ostream, ok??
			os << rhs.str(); return os; 
		}

		auto dim() const -> u32 {
			return DIM;
		}

		auto str() const -> const std::string  { 
			std::stringstream s;

			s << "vec" << DIM << "(";
			for (size_t i = 0; i < DIM-1; i++) {
				s << this->numbers[i] << ", ";
			}
			s << numbers[DIM-1] << ")";
			return s.str();
		}
		

		T* begin() { return (&numbers[0]); }
		T* end()   { return (&numbers[DIM]); } //
		
		T* data()  { return numbers.data(); }
	};

    struct vec2 : public vec<f32,3> {
        using MyBase   = vec<f32,3>;
        
        union {
            struct {f32 &x,&y;};
            struct {f32 &r,&g;};
            struct {f32 &u,&v;};
        };
        
        vec2& operator=(const vec2& other) { if (this != &other) { numbers = other.numbers; } return *this; }
        vec2(vec v) :MyBase(v),x(numbers[0]),y(numbers[1]){ };
        vec2() :MyBase(),x(numbers[0]),y(numbers[1]){ };
        vec2(std::initializer_list<f32> vals) :MyBase(vals),x(numbers[0]),y(numbers[1]){ };
        vec2(f32 x, f32 y) :MyBase(std::initializer_list<f32>({x,y})),x(numbers[0]),y(numbers[1]){ };

    };

	struct vec3 : public vec<f32,3> {
        using Type = f32;
		using MyBase   = vec<Type,3>;
		using ArrayType = std::array<Type,3>;
		
		union {
			struct {f32 &x,&y,&z;};
			struct {f32 &r,&g,&b;};
		};

        vec3& operator=(const vec3& other) {
            const u32 count = numbers.size();
            for (u32 i = 0; i < count; i++) {
                (*this)[i] = other[i];
            }
            return (*this);
        }

		vec3(Type * v) :MyBase(v),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		vec3(vec v) :MyBase(v),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		vec3() :MyBase(),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		vec3(std::initializer_list<f32> vals) :MyBase(vals),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		vec3(f32 x, f32 y, f32 z) :MyBase(std::initializer_list<f32>({x,y,z})),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		vec3(vec2 v, f32 z) :MyBase(std::initializer_list<f32>({v.x, v.y, z})),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
        vec3 cross(const vec3& other) const {
            return vec3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
        } 

        static vec3 cross(const vec& v1, const vec& v2) {
            return vec3(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
        }
        static vec3 left(const vec3& rotation) {
            float yaw = radians(rotation.y);
            float x = std::cos(yaw);
            float y = 0;
            float z = std::sin(yaw);

            return {-x, -y, -z};
        }

        static vec3 right(const vec3& rotation) {
            return -left(rotation);
        }
	};

	struct vec4 : public vec<f32,4> {
		using MyBase   = vec<f32,4>;
		
		union {
			struct {f32 &x,&y,&z,&w;};
			struct {f32 &r,&g,&b,&a;};
		};
		
		vec4(vec v) :MyBase(v),x(numbers[0]),y(numbers[1]),z(numbers[2]),w(numbers[3]){ };
		vec4() :MyBase(),x(numbers[0]),y(numbers[1]),z(numbers[2]),w(numbers[2]){ };
		vec4(std::initializer_list<f32> vals) :MyBase(vals),x(numbers[0]),y(numbers[1]),z(numbers[2]),w(numbers[2]){ };
		vec4(f32 x, f32 y, f32 z,f32 w) :MyBase(std::initializer_list<f32>({x,y,z,w})),x(numbers[0]),y(numbers[1]),z(numbers[2]),w(numbers[3]){ };
		vec4(vec3 v, f32 w) :MyBase(std::initializer_list<f32>({v.x, v.y, v.z,w})),x(numbers[0]),y(numbers[1]),z(numbers[2]),w(numbers[3]){ };
	};	

    struct veci3 : public vec<i32,3> {

        using Type   = i32;
		using MyBase = vec<i32,3>;
		
		union {
			struct {	i32 &x,&y,&z;};
			struct {	i32 &r,&g,&b;};
		};
		
        veci3 operator=(const veci3& other) { {x = other.x; y = other.y;z = other.z; } return *this; }
		veci3() :MyBase(),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		veci3(std::initializer_list<i32> vals) :MyBase(vals),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
		veci3(Type x, Type y, Type z) :MyBase(std::initializer_list<Type>({x,y,z})),x(numbers[0]),y(numbers[1]),z(numbers[2]){ };
	};

    struct veci2 : public vec<i32,3> {

        using Type = i32;
        using MyBase   = vec<i32,3>;
        
        union {
            struct {i32 &x,&y;};
            struct {i32 &r,&g;};
            struct {i32 &u,&v;};
            struct {i32 &width,&height;};
        };
        
        veci2& operator=(const veci2& other) { {x = other.x; y = other.y; } return *this; }
        veci2(vec v) :MyBase(v),x(numbers[0]),y(numbers[1]){ };
        veci2() :MyBase(),x(numbers[0]),y(numbers[1]){ };
        veci2(std::initializer_list<i32> vals) :MyBase(vals),x(numbers[0]),y(numbers[1]){ };
        veci2(i32 x, i32 y) :MyBase(std::initializer_list<i32>({x,y})),x(numbers[0]),y(numbers[1]){ };

    };
};
