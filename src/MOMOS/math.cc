#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>

#include <MOMOS/math.h>
#include <MOMOS/momos.h>

namespace MOMOS {

	Mat3 Mat3Multiply(const Mat3 &a, const Mat3 &b) {
		Mat3 momos_mat;

		momos_mat.d[0] = a.d[0] * b.d[0] + a.d[1] * b.d[3] + a.d[2] * b.d[6];
		momos_mat.d[1] = a.d[0] * b.d[1] + a.d[1] * b.d[4] + a.d[2] * b.d[7];
		momos_mat.d[2] = a.d[0] * b.d[2] + a.d[1] * b.d[5] + a.d[2] * b.d[8];

		momos_mat.d[3] = a.d[3] * b.d[0] + a.d[4] * b.d[3] + a.d[5] * b.d[6];
		momos_mat.d[4] = a.d[3] * b.d[1] + a.d[4] * b.d[4] + a.d[5] * b.d[7];
		momos_mat.d[5] = a.d[3] * b.d[2] + a.d[4] * b.d[5] + a.d[5] * b.d[8];

		momos_mat.d[6] = a.d[6] * b.d[0] + a.d[7] * b.d[3] + a.d[5] * b.d[6];
		momos_mat.d[7] = a.d[6] * b.d[1] + a.d[7] * b.d[4] + a.d[5] * b.d[7];
		momos_mat.d[8] = a.d[6] * b.d[2] + a.d[7] * b.d[5] + a.d[5] * b.d[8];

		return momos_mat;
	}

	void Mat3Multiply(const Mat3 &a, const Mat3 &b, Mat3* c) {
		*c = Mat3Multiply(a, b);
	}

	Mat3 Mat3Identity() {
		Mat3 momos_mat;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				momos_mat.d[i * 3 + j] = (i == j) ? 1.0f : 0.0f;
			}
		}

		return momos_mat;
	}


	Mat3 Mat3Translate(float translate_x, float translate_y) {
		Mat3 momos_mat = Mat3Identity();
		momos_mat.d[2] = translate_x;
		momos_mat.d[5] = translate_y;

		return momos_mat;
	}


	void Mat3InitAsTranslate(float x, float y, Mat3* mat) {
		*mat = Mat3Translate(x, y);
	}


	Mat3 Mat3Rotate(float radians) {
		Mat3 momos_mat = Mat3Identity();
		momos_mat.d[0] = cos(radians);
		momos_mat.d[1] = -sin(radians);
		momos_mat.d[3] = sin(radians);
		momos_mat.d[4] = cos(radians);

		return momos_mat;
	}


	void Mat3InitAsRotate(float radians, Mat3* mat) {
		*mat = Mat3Rotate(radians);
	}


	Mat3 Mat3Scale(float scale_x, float scale_y) {
		Mat3 momos_mat = Mat3Identity();
		
		momos_mat.d[0] = scale_x;
		momos_mat.d[3] = scale_y;

		return momos_mat;
	}


	void Mat3InitAsScale(float scale_x, float scale_y, Mat3* mat) {
		*mat = Mat3Scale(scale_x, scale_y);
	}


	Mat3 Mat3FromColumns(const float data[9]) {
		Mat3 momos_mat;
		for (int col = 0; col < 3; ++col) {
			for (int row = 0; row < 3; ++row) {
				momos_mat.d[row * 3 + col] = data[col * 3 + row];
			}
		}
		return momos_mat;
	}


	Vec3 Mat3TransformVec3(const Mat3 &a, const Vec3 &v) {
		Vec3 momos_vec;
		momos_vec.x = v.x * a.d[0] + v.y * a.d[1] + v.z * a.d[2];
		momos_vec.y = v.x * a.d[3] + v.y * a.d[4] + v.z * a.d[5];
		momos_vec.z = v.x * a.d[6] + v.y * a.d[7] + v.z * a.d[8];
		return momos_vec;
	}


	Vec2 Mat3TransformVec2(const Mat3 &a, const Vec2  &v) {
		Vec2 momos_vec;
		float v_in[2] = {v.x, v.y};
		float v_out[2];
		Mat3TransformRawVec2(a, v_in, v_out);
		momos_vec = { v_out[0], v_out[1] };
		return momos_vec;
	}


	void Mat3TransformVec2(const Mat3 &a, const float v[2], float v_output[2]) {
		Vec2 v_in = { v[0], v[1] };
		Vec2 v_out;

		v_out = Mat3TransformVec2(a, v_in);
		v_output[0] = v_out.x;
		v_output[1] = v_out.y;
	}


	Mat4 Mat4Multiply(const Mat4 &a, const Mat4 &b) {
		Mat4 momos_mat;
		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				float value = 0.0f;
				for (int k = 0; k < 4; ++k) {
					value += a.d[row * 4 + k] * b.d[k * 4 + col];
				}
				momos_mat.d[row * 4 + col] = value;
			}
		}
		return momos_mat;
	}


	Mat4 Mat4Identity() {
		Mat4 momos_mat;
		for (int row = 0; row < 4; ++row) {
			for (int col = 0; col < 4; ++col) {
				momos_mat.d[row * 4 + col] = (row == col) ? 1.0f : 0.0f;
			}
		}
		return momos_mat;
	}


	Mat4 Mat4Translate(float translate_x, float translate_y, float translate_z) {
		Mat4 momos_mat = Mat4Identity();
		momos_mat.d[3] = translate_x;
		momos_mat.d[7] = translate_y;
		momos_mat.d[11] = translate_z;
		return momos_mat;
	}


	Mat4 Mat4RotateX(float radians) {
		Mat4 momos_mat = Mat4Identity();
		float c = cos(radians);
		float s = sin(radians);
		momos_mat.d[5] = c;
		momos_mat.d[6] = -s;
		momos_mat.d[9] = s;
		momos_mat.d[10] = c;
		return momos_mat;
	}


	Mat4 Mat4RotateY(float radians) {
		Mat4 momos_mat = Mat4Identity();
		float c = cos(radians);
		float s = sin(radians);
		momos_mat.d[0] = c;
		momos_mat.d[2] = s;
		momos_mat.d[8] = -s;
		momos_mat.d[10] = c;
		return momos_mat;
	}


	Mat4 Mat4RotateZ(float radians) {
		Mat4 momos_mat = Mat4Identity();
		float c = cos(radians);
		float s = sin(radians);
		momos_mat.d[0] = c;
		momos_mat.d[1] = -s;
		momos_mat.d[4] = s;
		momos_mat.d[5] = c;
		return momos_mat;
	}


	Mat4 Mat4Scale(float scale_x, float scale_y, float scale_z) {
		Mat4 momos_mat = Mat4Identity();
		momos_mat.d[0] = scale_x;
		momos_mat.d[5] = scale_y;
		momos_mat.d[10] = scale_z;
		return momos_mat;
	}


	Mat4 Mat4FromColumns(const float data[16]) {
		Mat4 momos_mat;
		for (int col = 0; col < 4; ++col) {
			for (int row = 0; row < 4; ++row) {
				momos_mat.d[row * 4 + col] = data[col * 4 + row];
			}
		}
		return momos_mat;
	}


	Mat4 Mat4Projection() {
		Mat4 momos_mat = Mat4Identity();

		float left = 0.0f;
		float right = MOMOS::win_width;
		float bottom = 0.0f;
		float top = MOMOS::win_height;
		float near_plane = -1.0f;
		float far_plane = 1.0f;

		float width = right - left;
		float height = top - bottom;
		float depth = far_plane - near_plane;

		if (width == 0.0f || height == 0.0f || depth == 0.0f) {
			return momos_mat;
		}

		momos_mat.d[0] = 2.0f / width;
		momos_mat.d[5] = 2.0f / height;
		momos_mat.d[10] = -2.0f / depth;
		momos_mat.d[3] = -(right + left) / width;
		momos_mat.d[7] = -(top + bottom) / height;
		momos_mat.d[11] = -(far_plane + near_plane) / depth;

		return momos_mat;
	}


	Vec4 Mat4TransformVec4(const Mat4 &a, const Vec4 &v) {
		Vec4 momos_vec;
		Mat4TransformRawVec4(a, &v.x, &momos_vec.x);
		return momos_vec;
	}


	Vec3 Mat4TransformVec3(const Mat4 &a, const Vec3 &v) {
		float vec_in[4] = { v.x, v.y, v.z, 1.0f };
		float vec_out[4];
		Mat4TransformRawVec4(a, vec_in, vec_out);

		Vec3 momos_vec = { vec_out[0], vec_out[1], vec_out[2] };
		if (vec_out[3] != 0.0f && vec_out[3] != 1.0f) {
			momos_vec.x /= vec_out[3];
			momos_vec.y /= vec_out[3];
			momos_vec.z /= vec_out[3];
		}

		return momos_vec;
	}


	void Mat3TransformRawVec2(const Mat3 &m, const float v_in[2], float v_out[2]) {
		v_out[0] = v_in[0] * m.d[0] + v_in[1] * m.d[1] + 1 * m.d[2];
		v_out[1] = v_in[0] * m.d[3] + v_in[1] * m.d[4] + 1 * m.d[5];
	}


	void Mat3TransformRawVec3(const Mat3 &m, const float v_in[3], float v_out[3]) {
		v_out[0] = v_in[0] * m.d[0] + v_in[1] * m.d[1] + v_in[2] * m.d[2];
		v_out[1] = v_in[0] * m.d[3] + v_in[1] * m.d[4] + v_in[2] * m.d[5];
		v_out[2] = v_in[0] * m.d[6] + v_in[1] * m.d[7] + v_in[2] * m.d[8];
	}


	void Mat4TransformRawVec3(const Mat4 &m, const float v_in[3], float v_out[3]) {
		float extended[4] = { v_in[0], v_in[1], v_in[2], 1.0f };
		float result[4];
		Mat4TransformRawVec4(m, extended, result);
		float w = (result[3] == 0.0f) ? 1.0f : result[3];
		v_out[0] = result[0] / w;
		v_out[1] = result[1] / w;
		v_out[2] = result[2] / w;
	}


	void Mat4TransformRawVec4(const Mat4 &m, const float v_in[4], float v_out[4]) {
		v_out[0] = v_in[0] * m.d[0] + v_in[1] * m.d[1] + v_in[2] * m.d[2] + v_in[3] * m.d[3];
		v_out[1] = v_in[0] * m.d[4] + v_in[1] * m.d[5] + v_in[2] * m.d[6] + v_in[3] * m.d[7];
		v_out[2] = v_in[0] * m.d[8] + v_in[1] * m.d[9] + v_in[2] * m.d[10] + v_in[3] * m.d[11];
		v_out[3] = v_in[0] * m.d[12] + v_in[1] * m.d[13] + v_in[2] * m.d[14] + v_in[3] * m.d[15];
	}

} /* MOMOS */