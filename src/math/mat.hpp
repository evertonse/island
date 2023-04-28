#pragma once
#include <array>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>
#include "utils/common.h"
#include "math/vec.hpp"

namespace cyx {

    template <typename T, size_t ROWS, size_t COLS>
    class mat {
        using mat4 = mat<f32,4,4>;
        using mat3 = mat<f32,3,3>;
        using col_type = vec<T, ROWS> ;
		using row_type = vec<T,COLS>;
		using my_type = mat<T, ROWS, COLS> ;
		using trans_type = mat<T, COLS, ROWS>;
		using value_type = T;

    public:
        mat() : numbers() {}
        mat(T* numbers) : numbers(numbers) {}
        mat(const std::initializer_list<std::initializer_list<T>>& list) {
            if (list.size() != ROWS) {
                throw std::invalid_argument("Incorrect number of rows in initializer list.");
            }
            size_t row_index = 0;
            for (const auto& row : list) {
                size_t col_index = 0;
                for (const auto& value : row) {
                    this->operator()(row_index,col_index) = value;
                    col_index++;
                }
                row_index++;
            }
        }
        // Initialize matrix with an array of values
        mat(std::array<T, ROWS * COLS> values) {
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                numbers[i] = values[i];
            }
        }
        // Initialize the matrix with diagonal = value
        mat(T value) {
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                numbers[i] = static_cast<T>(0);
            }
            constexpr size_t min_number = std::min(ROWS,COLS); 

           for (size_t i = 0; i < min_number ; i++) {
               this->operator()(i,i) = value;
           }
            
        }
        // Constructor fr
        // Constructor from vec4
        mat(const vec4& v) {
            if (ROWS == 4 && COLS == 1) {
                numbers[0] = v.x;
                numbers[1] = v.y;
                numbers[2] = v.z;
                numbers[3] = v.w;
            }
            else {
                throw std::invalid_argument("Invalid dimensions for matrix constructor from vec4");
            }
        } // Constructor from vec4
        mat(const mat& other) {
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                numbers[i] = other.numbers[i];
            }
        }

        // Accessors
        T& operator()(size_t row, size_t col) {
            return numbers[row * COLS + col];
        }

        const T& operator()(size_t row, size_t col) const {
            return numbers[row * COLS + col];
        }

        // mat addition
        mat operator+(const mat& other) const {
            mat result;
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                result.numbers[i] = numbers[i] + other.numbers[i];
            }
            return result;
        }

        // mat subtraction
        mat operator-(const mat& other) const {
            mat result;
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                result.numbers[i] = numbers[i] - other.numbers[i];
            }
            return result;
        }

        // Scalar multiplication
        mat operator*(const T& scalar) const {
            mat result;
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                result.numbers[i] = numbers[i] * scalar;
            }
            return result;
        }

        // mat multiplication
        template <size_t M>
        mat<T, ROWS, M> operator%(const mat<T, COLS, M>& other) {
            mat<T, ROWS, M> result;
            auto& self = *this;
            for (size_t i = 0; i < ROWS; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    T sum = 0;
                    for (size_t k = 0; k < COLS; ++k) {
                        sum += (self)(i, k) * other(k, j);
                    }
                    result(i, j) = sum;
                }
            }
            return result;
        }
        // mat multiplication
        vec4 operator%(const vec4& rhs) const {
            assert((ROWS == COLS) && COLS == 4);
            mat<f32,4,1> colmun_vec(rhs);
            return (*this) % colmun_vec;
        }

		inline operator vec4 () { 
            assert(numbers.size() == 4);
            return vec4(numbers[0],numbers[1], numbers[2], numbers[3]);
        }
        
        //template <typename T, size_t ROWS, size_t COLS>
        static mat identity() {
            mat m; 
            constexpr size_t min_number = std::min(ROWS,COLS); 
            for (size_t i = 0; i < ROWS; i++) {
                for (size_t j = 0; j < COLS; j++) {
                    m(i,j) = (i==j) ? (T)1:(T)0.0f; 
                }
            }
            
            return m;
        }

        static mat& identity(mat4& src) {
            constexpr size_t min_number = std::min(ROWS,COLS); 
            for (size_t i = 0; i < ROWS; i++) {
                for (size_t j = 0; j < COLS; j++) {
                    src(i,j) = (i==j) ? (T)1:(T)0.0f; 
                }
            }
        }

        // transpose this matrix
        mat<T, COLS, ROWS> transpose() const {
            mat<T, COLS, ROWS> result;
            for (size_t i = 0; i < ROWS; i++) {
                for (size_t j = 0; j < COLS; j++) {
                    result(j,i) = (*this)(i,j);
                }
            }
            return result;
        }

        static mat<f32,4,4> perspective2(
            f32 fov, f32 aspect_ratio, f32 near_plane, f32 far_plane){

            // Calculate the projection matrix
            const float tan_half_fov = std::tan(fov / 2.0f);
            const float right = tan_half_fov * near_plane * aspect_ratio;
            const float left = -right;
            const float top = tan_half_fov * near_plane;
            const float bottom = -top;

            const float m00 = (2.0f * near_plane) / (right - left);
            const float m11 = (2.0f * near_plane) / (top - bottom);
            const float m22 = -(far_plane + near_plane) / (far_plane - near_plane);
            const float m23 = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
            const float m32 = -1.0f;

            mat4 projection_matrix = mat4::identity();
            projection_matrix(0,0) = m00;
            projection_matrix(1,1) = m11;
            projection_matrix(2,2) = m22;
            projection_matrix(2,3) = m23;
            projection_matrix(3,2) = m32;
            return projection_matrix;
        }

		T& operator[]  (i32 index) { return numbers[index]; }
		const T& operator[]  (i32 index) const { return numbers[index]; }
		mat& operator=(const mat& mat) = default;     
		T* begin() { return (&numbers[0]); }
		T* end()   { return (&numbers[numbers.size()]); } //
		T* data()  { return numbers.data(); }

        static mat4 frustum(
            f32 left, f32 right, f32 bottom,
            f32 top, f32 znear, f32 zfar)
        {
            assert(0 && "Make sure this function is row major");
            mat4 matrix;
            float temp, temp2, temp3, temp4;
            temp = 2.0 * znear;
            temp2 = right - left;
            temp3 = top - bottom;
            temp4 = zfar - znear;
            matrix[0] = temp / temp2;
            matrix[1] = 0.0;
            matrix[2] = 0.0;
            matrix[3] = 0.0;

            matrix[4] = 0.0;
            matrix[5] = temp / temp3;
            matrix[6] = 0.0;
            matrix[7] = 0.0;

            matrix[8] = (right + left) / temp2;
            matrix[9] = (top + bottom) / temp3;
            matrix[10] = (-zfar - znear) / temp4;

            matrix[11] = -1.0;
            matrix[12] = 0.0;
            matrix[13] = 0.0;
            matrix[14] = (-temp * zfar) / temp4;

            matrix[15] = 0.0;
            return matrix;
        }

        static mat4 perspective(
            f32 fov, f32 aspect,
            f32 znear, f32 zfar)
        {
            mat4 m(f32(0));
            const f32 t = std::tan(fov/2.0f);

            m(0,0) = 1.0f/(aspect * t);
            m(1,1) = 1.0f/(t);
            m(2,2) = -((zfar + znear)/(zfar-znear));
            m(3,2) = -1.0f;
            m(2,3) = -((2*zfar*znear)/(zfar-znear));
            m(3,3) = 0.0f;
            return m;
        }
        static mat4 lookat(const vec3& eye, const vec3& center, const vec3& up) {
            vec3 f((center - eye).normalize());
            vec3 s(vec3::cross(up, f).normalize());
            vec3 u(vec3::cross(f, s));

            mat4 m = mat4::identity();
            m(0,0) = s.x;
            m(0,1) = s.y;
            m(0,2) = s.z;
            m(1,0) = u.x;
            m(1,1) = u.y;
            m(1,2) = u.z;
            m(2,0) = f.x;
            m(2,1) = f.y;
            m(2,2) = f.z;
            m(0,3) = -vec3::dot(s, eye);
            m(1,3) = -vec3::dot(u, eye);
            m(2,3) = -vec3::dot(f, eye);
            return m;
        }

        static mat4 ortho(
            f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
        {
            assert(0 && "Make sure this function is row major");
            mat4 m;
            m[0] = 2 / (right - left);
            m[1] = 0;
            m[2] = 0;
            m[3] = 0;
            m[4] = 0;
            m[5] = 2 / (top - bottom);
            m[6] = 0;
            m[7] = 0;
            m[8] = 0;
            m[9] = 0;
            m[10] = -2 / (far - near);
            m[11] = 0;
            m[12] = -(right + left) / (right - left);
            m[13] = -(top + bottom) / (top - bottom);
            m[14] = -(far + near) / (far - near);
            m[15] = 1;
            return m;
        }

        static mat<f32, 4, 4> projective_transform(
            const mat<f32, 3, 3>& linear, 
            const mat<f32, 3, 1>& translation, 
            const mat<f32, 3, 1>& perspective, 
            f32 scale) {
            return mat<f32, 4, 4>({
                {linear(0, 0), linear(0, 1), linear(0, 2), translation(0,0)},
                {linear(1, 0), linear(1, 1), linear(1, 2), translation(1,0)},
                {linear(2, 0), linear(2, 1), linear(2, 2), translation(2,0)},
                {perspective(0,0), perspective(1,0), perspective(2,0), scale}
            });
        }

        static mat<f32, 4, 4> projective_transform(
            const mat<f32, 3, 3>& linear, 
            const vec3& translation, 
            const vec3& perspective, 
            f32 scale) 
        {
            return mat<f32, 4, 4>({
                {linear(0, 0), linear(0, 1), linear(0, 2), translation[0]},
                {linear(1, 0), linear(1, 1), linear(1, 2), translation[1]},
                {linear(2, 0), linear(2, 1), linear(2, 2), translation[2]},
                {perspective[0], perspective[1], perspective[2], scale  }
            });
        }




        static mat4 scaling(f32 scaling_factor) {
            mat4 m = mat::identity();
            for (size_t i = 0; i < 3; i++) {
                m(i,i) = scaling_factor;
            }
            m(3,3) = 1.0f;
            return m;
        }
        static mat4 scale(const mat4& m, f32 scaling_factor) {
            mat4 id = mat::identity();
            id(3,3) = 1/scaling_factor;

            return scaling(scaling_factor) % m;
        }
         
        static mat4 rotate(const mat& m, f32 angle, const vec3 & axis) {
            mat4 rotate = mat::rotation(angle,axis);
            mat4 result = rotate % m; 
            return result; 
        }

        static mat4 rotation(f32 angle, const vec3 & axis) {
            f32 const c = std::cos(angle);
            f32 const s = std::sin(angle);

            vec3 axis_ = axis.normalize();

            vec3 temp = (axis_ * (f32(1.0) - c));

            mat4 m(1.0f);
            m(0,0) = temp(0) * axis_(0) + c;
            m(1,0) = temp(0) * axis_(1) + s * axis_(2);
            m(2,0) = temp(0) * axis_(2) - s * axis_(1);

            m(0,1) = temp(1) * axis_(0) - s * axis_(2);
            m(1,1) = temp(1) * axis_(1) + c;
            m(2,1) = temp(1) * axis_(2) + s * axis_(0);

            m(0,2) = temp(2) * axis_(0) + s * axis_(1);
            m(1,2) = temp(2) * axis_(1) - s * axis_(0);
            m(2,2) = temp(2) * axis_(2) + c;

            return m;
        }

        static mat4 translate(const mat4 & m, const vec3 & v) {
            return translation(v) % m ;
        }

        static mat4 translation(const vec3& t) {
            mat4 m = mat4::identity();
            m(0,3) = t.x;
            m(1,3) = t.y;
            m(2,3) = t.z;
            return m;
        }

        // Output matrix to stream
        friend std::ostream& operator<<(std::ostream& os, const mat& matrix) {

            os << '['; 
            for (size_t i = 0; i < ROWS; ++i) {
                os << '['; 
                for (size_t j = 0; j < COLS; ++j) {
                    os << matrix(i, j) << ' ';
                }
                os << ']' << ','; 
                os << '\n';
            }

            os << ']'; 
            return os;
        }

        private:
        std::array<T, ROWS * COLS> numbers;
    };
    using mat4 = mat<f32,4,4>;
    using mat3 = mat<f32,3,3>;
}