#include <array>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include <utility>
#include <type_traits>
#include "utils/common.h"
#include "math/useful.hpp"


namespace cyx {
    // *** This idead is from https://seanmiddleditch.github.io/c-method-type-deduction-tricks/
    template <typename T, u32 DIM> 
    struct VecData {
        std::array<T, DIM> numbers;
    };

    template <typename T> 
    struct VecData<T,2> {
        union {
            std::array<T, 2> numbers;
            struct { T x, y; };
            struct { T r, g; };
            struct { T u, v; };
            struct { T s, t; };
        };
    };

    template <typename T> 
    struct VecData<T,3> {
        union {
            std::array<T, 3> numbers;
            struct { T x, y, z; };
            struct { T r, g, b; };
            struct { T s, t, p; };
            struct { T u, v; };
        };
    };

    template <typename T> 
    struct VecData<T,4> {
        union {
            std::array<T, 4> numbers;
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
            struct { T s, t, p, q; };
            struct { T u, v; };
        };
    };

	template <typename T, u32 DIM>
	struct vec : public VecData<T,DIM> {
        using VecData<T, DIM>::numbers;

        using vec_concrete = vec<T,DIM>;
        using vec2 = vec<f32,2>;
        using vec3 = vec<f32,3>;
        using vec4 = vec<f32,4>;

        using veci2 = vec<i32,2>;
        using veci3 = vec<i32,3>;
        using veci4 = vec<i32,4>;

		vec() {}
        vec& operator=(const vec& other) {
            if (this != &other) {
                for (size_t i = 0; i < DIM; i++){
                    (*this)[i] = other[i];
                }
            }
            return *this;
        }

        template< typename... Args>
        explicit vec(Args... args) {
            static_assert(sizeof...(Args) == DIM, "size of Args != from vec dimension");
            T temp[DIM] = { std::forward<Args>(args)... };
            for (size_t i = 0; i < DIM; i++) {
                this->numbers[i] = temp[i];
            }
        }

        // We need to create another generic constructor because the copy constructor is not
        // being called when the more generic Args... constructor exists, it keeps trying to use
        // the Args... one,
        template<typename U>
        explicit vec(const vec<U,DIM> other) {
            for (size_t i = 0; i < DIM; i++) {
                this->numbers[i] = (T)other.numbers[i]; 
            }
        }

		vec(const vec& other) {
            numbers = other.numbers;
        }

        vec(vec&& other) {
            numbers = std::move(other.numbers);
        }

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


		vec  operator *  (const T& rhs) const { 		
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
		bool operator==(const vec& other) const {
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

   using vec2 = vec<f32, u32(2)>;
   using vec3 = vec<f32, u32(3)>;
   using vec4 = vec<f32, u32(4)>;

   using veci2 = vec<i32,u32(2)>;
   using veci3 = vec<i32,u32(3)>;
   using veci4 = vec<i32,u32(4)>;
        
};
