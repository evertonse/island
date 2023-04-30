#pragma once
#include <array>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <utility>
#include "utils/common.h"
#include "math/useful.hpp"
#define CYX_USE_REFERENCE_VEC

namespace cyx {

	template <class T, u32 DIM, typename Enable = void>
	class vec {
	
	public:
    #if defined(CYX_USE_REFERENCE_VEC) 
	    std::array<T,DIM> numbers;

        #if !defined(CYX_VEC_SPECIALIZATION)
        T& x = numbers[0];
        T& y = numbers[1];
        T& z = DIM == 2 ? numbers[1]:numbers[2]; 
        T& w = DIM == 2 ? numbers[1]: DIM == 2? numbers[2] : numbers[3]; 

        T& u = x;
        T& v = y;

        T& width = x;
        T& height = y;

        T& r = x;
        T& g = y;
        T& b = z;
        T& a = w;
        #endif
    #else
        union {
            std::array<T, DIM> numbers;
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
            struct { T u, v; };
            struct { T s, t, p, q; };
        };
    #endif
        using vec_concrete = vec<T,DIM>;
        using vec2 = vec<f32,2>;
        using vec3 = vec<f32,3>;
        using vec4 = vec<f32,4>;

        using veci2 = vec<i32,2>;
        using veci3 = vec<i32,3>;
        using veci4 = vec<i32,4>;

	public:
		vec() {}
        vec& operator=(const vec& other) {
            if (this != &other) {
                for (size_t i = 0; i < DIM; i++){
                    (*this)[i] = other[i];
                }
            }
            return *this;
        }

        // TODO  add support for default init if size of args < DIM
        template<typename... Args>
        vec(Args... args) {
            static_assert(sizeof...(args) == DIM, " Args != from DIM");
            T temp[DIM] = { args... };
            for (size_t i = 0; i < DIM; i++) {
                this->numbers[i] = temp[i];
            }
            
        }

        //template <typename AnotherType> 
        //vec_concrete& operator=(const vec<AnotherType, DIM>& other){
            //size_t count = this->numbers.size();
            //for (size_t i = 0; i < count; i++) {
                //(*this)[i] = static_cast<T>(other[i]);
            //} 
            //return *this;
        //}

		vec(const vec<T, DIM>& other) : numbers(other.numbers) {};
        vec(vec<T, DIM>&& other) : numbers(std::move(other.numbers)) {}

        vec& operator=(vec&& other) noexcept {
            if (this != &other) {
                numbers = std::move(other.numbers);
            }
            return *this;
        }

		vec(std::initializer_list<T> values){
			size_t index = 0;
			for (auto& val : values) {
				numbers[index] = val;
                index++;
			}
		}
        
        vec(T *vals) {
           for(size_t i = 0; i < DIM; i++){
                numbers[i] = vals[i];
           } 
        }


        vec(const T *vals) {
           for(size_t  i = 0; i < DIM; i++){
                numbers[i] = vals[i];
           } 
        }

        
        template <typename AnotherType> 
        vec(const vec<AnotherType, DIM>& other){
            for(size_t   i = 0; i < DIM; i++) {
                numbers[i] = (T)other[i];
            } 
        }

		T& operator[]  (i32 index) { return numbers[index]; }
		const T& operator[]  (i32 index) const { return numbers[index]; }
		T& operator()  (i32 index) { return numbers[index]; }
		const T& operator()  (i32 index) const { return numbers[index]; }

        // Copy assignment operator
        //vec& operator=(const vec& other) { if (this != &other) { numbers = other.numbers; } return *this; }
		//vec operator=(const vec& v) = default;



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

        static f32 length(vec other ) {
            return std::sqrt((f32)other.dot(other));
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
				new_vec[i] = (*this)[i] - other[i] ;
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
	
		bool operator != (const vec& other) const { 
            for (size_t i = 0; i < DIM; i++) {
                if((*this)[i] != other[i])
                    return false;  
            }
			return true; 
        }

		
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

			//s << "vec" << DIM << "("; // opting for showing the dimension but I find it confusing alotta times
			s << "vec" << "(";
			for (size_t i = 0; i < DIM-1; i++) {
				s << this->numbers[i] << ", ";
			}
			s << numbers[DIM-1] << ")";
			return s.str();
		}
		

		T* begin() { return (&numbers[0]); }
		T* end()   { return (&numbers[DIM]); } //
		
		T* data()  { return numbers.data(); }

        vec3 cross(const vec3& other) const {
            vec3& v1 = this;
            vec3& v2 = other;
            return vec3(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
        } 

        static vec3 cross(const vec& v1, const vec& v2) {
            return vec3(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
        }
        static vec3 left(const vec3& rotation) {
            float yaw = radians(rotation[1] + 90.0f);
            float x = std::cos(yaw);
            float y = 0;
            float z = std::sin(yaw);

            return {-x, -y, -z};
        }

        static vec3 right(const vec3& rotation) {
            return -left(rotation);
        }

        static f32 angle(const vec3& v1, const vec3& v2) {
            f32 dot_product = v1.dot(v2);
            float magnitude_product = length(v1) * length(v2);
            float cos_theta = dot_product / magnitude_product;
            float theta = std::acos(cos_theta);
            return theta;
        }
	};

#if defined (CYX_VEC_SPECIALIZATION)
    template <class T, u32 DIM>
    class vec<T, DIM, typename std::enable_if<DIM == u32(4)>::type> {
    public:
        std::array<T,DIM> numbers;
        T& x = numbers[0];
        T& y = numbers[1];
        T& z = numbers[2];
        T& w = numbers[3];

        T& u = x;
        T& v = y;

        T& width = x;
        T& height = y;

        T& r = x;
        T& g = y;
        T& b = z;
        T& a = w;
    };

    template <class T, u32 DIM>
    class vec<T, DIM, typename std::enable_if<DIM == u32(3)>::type> {
    public:
        std::array<T,DIM> numbers;
        T& x = numbers[0];
        T& y = numbers[1];
        T& z = numbers[2];

        T& u = x;
        T& v = y;

        T& width = x;
        T& height = y;

        T& r = x;
        T& g = y;
        T& b = z;
    };

    template <class T, u32 DIM>
    class vec<T, DIM, typename std::enable_if<DIM == u32(2)>::type> {
    public:
        std::array<T,DIM> numbers;
        T& x = numbers[0];
        T& y = numbers[1];

        T& u = x;
        T& v = y;

        T& width = x;
        T& height = y;

        T& r = x;
        T& g = y;
    };
#endif

   using vec2 = vec<f32, u32(2)>;
   using vec3 = vec<f32, u32(3)>;
   using vec4 = vec<f32, u32(4)>;

   using veci2 = vec<i32,u32(2)>;
   using veci3 = vec<i32,u32(3)>;
   using veci4 = vec<i32,u32(4)>;
        
};
