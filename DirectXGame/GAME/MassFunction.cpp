#include <cmath>
#include <assert.h>
#include <numbers>
#include "MassFunction.h"


//02_06のCameraControllerのUpdate/Reset関数で必要
const Vector3 operator+(const Vector3 &v1, const Vector3 &v2) {
	Vector3 temp(v1);
	return temp += v2;
}

//02_06のスライド24枚目のLerp関数
Vector3 Lerp(const Vector3 &v1, const Vector3 &v2, float t) {
	return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t));
}

Vector3& operator+=(Vector3& lhv, const Vector3& rhv) {
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

Vector3& operator-=(Vector3& lhv, const Vector3& rhv) {
	lhv.x -= rhv.x;
	lhv.y -= rhv.y;
	lhv.z -= rhv.z;
	return lhv;
}

Vector3& operator*=(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

Vector3& operator/=(Vector3& v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}
//02_06の29枚目(CameraControllerのUpdate)で必要
const Vector3 operator*(const Vector3 &v1, const float f) {
	Vector3 temp(v1);
	return temp *= f;
}

float EaseIn(float x1, float x2, float t) {
	float easedT = t * t;

	return Lerp(x1, x2, easedT);
}
float EaseOut(float x1, float x2, float t) {
	float easedT = 1.0f - std::powf(1.0f - t, 3.0f);

	return Lerp(x1, x2, easedT);
}

float Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; }
float Radian(float degree) { 

	
return degree*(std::numbers::pi_v<float>/180.0f); 

}
bool IsCollision(const AABB& aabb1, const AABB& aabb2) { 
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && // x軸
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && // y軸
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);   // z軸
}
float ToRadian(float degree) {


	if (degree < 0.0f || degree > 360.0f) {
		return 0.0f; // 無効な値の場合は0を返す
	}
	return degree * (std::numbers::pi_v<float> / 180.0f);
}
float EaseInOut(float x1, float x2, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> *t) - 1.0f) / 2.0f;

	return Lerp(x1, x2, easedT);

	
	
}


	void WorldTransformUpdate(WorldTransform* worldTransform) {
		Matrix4x4 affin_mat = MakeAfineMatrix(
		worldTransform->scale_,
		worldTransform->rotation_,
		worldTransform->translation_
	);
		
		worldTransform->matWorld_ = affin_mat;
	    worldTransform->TransferMatrix();
	}

Matrix4x4 MakeAfineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& traslate)
	{
		Matrix4x4 scaleMatrix=MakeScaleMatrix(scale);
		Matrix4x4 rotateMatrix=Multiply(MakeRotateXMatrix( rotate.x),Multiply(MakeRotateYMatrix( rotate.y),MakeRotateZMatrix( rotate.z)));
		Matrix4x4 traslateMatrix=MakeTranslateMatrix(traslate);

		Matrix4x4 result=Multiply(Multiply(scaleMatrix,rotateMatrix),traslateMatrix);

		return result ;
	}

	/// <summary>
/// 
/// </summary>
/// <param name="traslate"></param>
/// <returns></returns>
Matrix4x4 MakeTranslateMatrix(const Vector3& traslate){
	return Matrix4x4(
		{
			{
				{1.0f,0.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f,0.0f},
				{0.0f,0.0f,1.0f,0.0f},
				{traslate.x,traslate.y,traslate.z,1.0f}
			}
		}
	);
}
/// <summary>
/// 
/// </summary>
/// <param name="scale"></param>
/// <returns></returns>
Matrix4x4 MakeScaleMatrix(const Vector3& scale){
	return Matrix4x4(
		{
			{
				{scale.x,0.0f,0.0f,0.0f},
				{0.0f,scale.y,0.0f,0.0f},
				{0.0f,0.0f,scale.z,0.0f},
				{0.0f,0.0f,0.0f,1.0f}
			}
		}
	);
}
/// <summary>
/// 
/// </summary>
/// <param name="vector"></param>
/// <param name="matrix"></param>
/// <returns></returns>
Vector3 Transform(const Vector3& vector,const Matrix4x4& matrix ){
	Vector3 result ;
	result.x=vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] +vector.z*matrix.m[2][0]+ 1.0f * matrix.m[3][0];
	result.y=vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] +vector.z*matrix.m[2][1]+ 1.0f * matrix.m[3][1];
	result.z=vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] +vector.z*matrix.m[2][2]+ 1.0f * matrix.m[3][2];
	float  w=vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] +vector.z*matrix.m[2][3]+ 1.0f * matrix.m[3][3];
	if (true)
	{

	}

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}
/// <summary>
/// 
/// </summary>
/// <param name="radian"></param>
/// <returns></returns>
Matrix4x4 MakeRotateXMatrix(float radian)
{
	return Matrix4x4(
	{
		{1.0f,0.0f,0.0f,0.0f,},
		{0.0f,std::cos(radian),std::sin(radian),0.0f},	
		{0.0f,-std::sin(radian),std::cos(radian),0.0f},
		{0.0f,0.0f,0.0f,1.0f}
	}
	);
}
Matrix4x4 MakeRotateYMatrix(float radian)
{
	return Matrix4x4(
		{
		{std::cos(radian),0.0f,-std::sin(radian),0.0f,},
		{0.0f,1.0f,0.0f,0.0f},	
		{std::sin(radian),0.0f,std::cos(radian),0.0f},
		{0.0f,0.0f,0.0f,1.0f}
	}
	);
}
Matrix4x4 MakeRotateZMatrix(float radian)
{
	return Matrix4x4(
	{
		
		{std::cos(radian),std::sin(radian),0.0f,0.0f},	
		{-std::sin(radian),std::cos(radian),0.0f,0.0f},
		{0.0f,0.0f,1.0f,0.0f,},
		{0.0f,0.0f,0.0f,1.0f}
	}
	);
}
//
	Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2){
		Matrix4x4 result={};
		for (int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				result.m[i][j]=m1.m[i][j]+m2.m[i][j];
			}
		}
		return result;
	}
	Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2){
		Matrix4x4 result={};
		for (int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				result.m[i][j]=m1.m[i][j]-m2.m[i][j];
			}
		}
		return result;
	}
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2){
		Matrix4x4 result={};
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					result.m[i][j] += m1.m[i][k] * m2.m[k][j];
				}
			}
		}
		
		return result;
	}
	Matrix4x4 Inverse(const Matrix4x4& m){
			float det =
				(m.m[0][0]*m.m[1][1]*m.m[2][2]*m.m[3][3])+(m.m[0][0]*m.m[1][2]*m.m[2][3]*m.m[3][1])+(m.m[0][0]*m.m[1][3]*m.m[2][1]*m.m[3][2])
			-	(m.m[0][0]*m.m[1][3]*m.m[2][2]*m.m[3][1])-(m.m[0][0]*m.m[1][2]*m.m[2][1]*m.m[3][3])-(m.m[0][0]*m.m[1][1]*m.m[2][3]*m.m[3][2])
			-	(m.m[0][1]*m.m[1][0]*m.m[2][2]*m.m[3][3])-(m.m[0][2]*m.m[1][0]*m.m[2][3]*m.m[3][1])-(m.m[0][3]*m.m[1][0]*m.m[2][1]*m.m[3][2])
			+	(m.m[0][3]*m.m[1][0]*m.m[2][2]*m.m[3][1])+(m.m[0][2]*m.m[1][0]*m.m[2][1]*m.m[3][3])+(m.m[0][1]*m.m[1][0]*m.m[2][3]*m.m[3][2])
			+	(m.m[0][1]*m.m[1][2]*m.m[2][0]*m.m[3][3])+(m.m[0][2]*m.m[1][3]*m.m[2][0]*m.m[3][1])+(m.m[0][3]*m.m[1][1]*m.m[2][0]*m.m[3][2])
			-	(m.m[0][3]*m.m[1][2]*m.m[2][0]*m.m[3][1])-(m.m[0][2]*m.m[1][1]*m.m[2][0]*m.m[3][3])-(m.m[0][1]*m.m[1][3]*m.m[2][0]*m.m[3][2])
			-	(m.m[0][1]*m.m[1][2]*m.m[2][3]*m.m[3][0])-(m.m[0][2]*m.m[1][3]*m.m[2][1]*m.m[3][0])-(m.m[0][3]*m.m[1][1]*m.m[2][2]*m.m[3][0])
			+	(m.m[0][3]*m.m[1][2]*m.m[2][1]*m.m[3][0])+(m.m[0][2]*m.m[1][1]*m.m[2][3]*m.m[3][0])+(m.m[0][1]*m.m[1][3]*m.m[2][2]*m.m[3][0])
			;
		Matrix4x4 result=
		{
			{
			//0
				{///0
					(1 / det)*(
						 (m.m[1][1]*(m.m[2][2]*m.m[3][3]-m.m[2][3]*m.m[3][2]))
						+(m.m[1][2]*(m.m[2][3]*m.m[3][1]-m.m[2][1]*m.m[3][3]))
						+(m.m[1][3]*(m.m[2][1]*m.m[3][2]-m.m[2][2]*m.m[3][1]))
						),
				////

				 ///1
					-(1 / det)*(
						 (m.m[2][1]*(m.m[3][2]*m.m[0][3]-m.m[3][3]*m.m[0][2]))
						+(m.m[2][2]*(m.m[3][3]*m.m[0][1]-m.m[3][1]*m.m[0][3]))
						+(m.m[2][3]*(m.m[3][1]*m.m[0][2]-m.m[3][2]*m.m[0][1]))
					),
				////
		
				///2,
					(1 / det)*(
						 (m.m[3][1]*(m.m[0][2]*m.m[1][3]-m.m[0][3]*m.m[1][2]))
						+(m.m[3][2]*(m.m[0][3]*m.m[1][1]-m.m[0][1]*m.m[1][3]))
						+(m.m[3][3]*(m.m[0][1]*m.m[1][2]-m.m[0][2]*m.m[1][1]))
						),
				////

				///3
					-(1 / det)*(
						 (m.m[0][1]*(m.m[1][2]*m.m[2][3]-m.m[1][3]*m.m[2][2]))
						+(m.m[0][2]*(m.m[1][3]*m.m[2][1]-m.m[1][1]*m.m[2][3]))
						+(m.m[0][3]*(m.m[1][1]*m.m[2][2]-m.m[1][2]*m.m[2][1]))
						),
				////
				},
			////
			
			//1
				{
				///0
				-(1 / det)*(
						 (m.m[1][2]*(m.m[2][3]*m.m[3][0]-m.m[2][0]*m.m[3][3]))
						+(m.m[1][3]*(m.m[2][0]*m.m[3][2]-m.m[2][2]*m.m[3][0]))
						+(m.m[1][0]*(m.m[2][2]*m.m[3][3]-m.m[2][3]*m.m[3][2]))
						),
				////

				///1
				(1 / det)*(
						 (m.m[2][2]*(m.m[3][3]*m.m[0][0]-m.m[3][0]*m.m[0][3]))
						+(m.m[2][3]*(m.m[3][0]*m.m[0][2]-m.m[3][2]*m.m[0][0]))
						+(m.m[2][0]*(m.m[3][2]*m.m[0][3]-m.m[3][3]*m.m[0][2]))
						),
				////

				///2
				-(1 / det)*(
						 (m.m[3][2]*(m.m[0][3]*m.m[1][0]-m.m[0][0]*m.m[1][3]))
						+(m.m[3][3]*(m.m[0][0]*m.m[1][2]-m.m[0][2]*m.m[1][0]))
						+(m.m[3][0]*(m.m[0][2]*m.m[1][3]-m.m[0][3]*m.m[1][2]))
						),
				///3
				(1 / det)*(
						 (m.m[0][2]*(m.m[1][3]*m.m[2][0]-m.m[1][0]*m.m[2][3]))
						+(m.m[0][3]*(m.m[1][0]*m.m[2][2]-m.m[1][2]*m.m[2][0]))
						+(m.m[0][0]*(m.m[1][2]*m.m[2][3]-m.m[1][3]*m.m[2][2]))
						),
				////

				},
			////
			
			//2
				{
				///0
				(1 / det)*(
						 (m.m[1][3]*(m.m[2][0]*m.m[3][1]-m.m[2][1]*m.m[3][0]))
						+(m.m[1][0]*(m.m[2][1]*m.m[3][3]-m.m[2][3]*m.m[3][1]))
						+(m.m[1][1]*(m.m[2][3]*m.m[3][0]-m.m[2][0]*m.m[3][3]))
						),
				////

				///1
				-(1 / det)*(
						 (m.m[2][3]*(m.m[3][0]*m.m[0][1]-m.m[3][1]*m.m[0][0]))
						+(m.m[2][0]*(m.m[3][1]*m.m[0][3]-m.m[3][3]*m.m[0][1]))
						+(m.m[2][1]*(m.m[3][3]*m.m[0][0]-m.m[3][0]*m.m[0][3]))
						),
				////
				
				///2
				(1 / det)*(
						 (m.m[3][3]*(m.m[0][0]*m.m[1][1]-m.m[0][1]*m.m[1][0]))
						+(m.m[3][0]*(m.m[0][1]*m.m[1][3]-m.m[0][3]*m.m[1][1]))
						+(m.m[3][1]*(m.m[0][3]*m.m[1][0]-m.m[0][0]*m.m[1][3]))
						),
				////
			
				///3
				-(1 / det)*(
						 (m.m[0][3]*(m.m[1][0]*m.m[2][1]-m.m[1][1]*m.m[2][0]))
						+(m.m[0][0]*(m.m[1][1]*m.m[2][3]-m.m[1][3]*m.m[2][1]))
						+(m.m[0][1]*(m.m[1][3]*m.m[2][0]-m.m[1][0]*m.m[2][3]))
						),
				},
			////
			
			//3
				{
				///0
				-(1 / det)*(
						 (m.m[1][0]*(m.m[2][1]*m.m[3][2]-m.m[2][2]*m.m[3][1]))
						+(m.m[1][1]*(m.m[2][2]*m.m[3][0]-m.m[2][0]*m.m[3][2]))
						+(m.m[1][2]*(m.m[2][0]*m.m[3][1]-m.m[2][1]*m.m[3][0]))
						),
				///1
				(1 / det)*(
						 (m.m[2][0]*(m.m[3][1]*m.m[0][2]-m.m[3][2]*m.m[0][1]))
						+(m.m[2][1]*(m.m[3][2]*m.m[0][0]-m.m[3][0]*m.m[0][2]))
						+(m.m[2][2]*(m.m[3][0]*m.m[0][1]-m.m[3][1]*m.m[0][0]))
						),
				///2
				-(1 / det)*(
						 (m.m[3][0]*(m.m[0][1]*m.m[1][2]-m.m[0][2]*m.m[1][1]))
						+(m.m[3][1]*(m.m[0][2]*m.m[1][0]-m.m[0][0]*m.m[1][2]))
						+(m.m[3][2]*(m.m[0][0]*m.m[1][1]-m.m[0][1]*m.m[1][0]))
						),
				///3
				(1 / det)*(
						 (m.m[0][0]*(m.m[1][1]*m.m[2][2]-m.m[1][2]*m.m[2][1]))
						+(m.m[0][1]*(m.m[1][2]*m.m[2][0]-m.m[1][0]*m.m[2][2]))
						+(m.m[0][2]*(m.m[1][0]*m.m[2][1]-m.m[1][1]*m.m[2][0]))
						),
				}
			}
		};
		
		
	

		return result;
	}
	Matrix4x4 Transpose(const Matrix4x4& m)
	{
		
		return Matrix4x4(
		{
			
				{m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0]},
				{m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1]},
				{m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2]},
				{m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]}
		}
		);
	}
	Matrix4x4 Makeidetity4x4(){
		return Matrix4x4(
			{
				{1,0,0,0},
				{0,1,0,0},
				{0,0,1,0},
				{0,0,0,1}
			}
		
		);
	}
//}//namespace KamataEngine{

	//加算
	Vector3 Add(const Vector3& v1, const Vector3& v2){
		Vector3 result={
			v1.x + v2.x,
			v1.y + v2.y,
			v1.z + v2.z
		
		};
		return result;
	}
	//
	Vector3 Subtract(const Vector3& v1, const Vector3& v2){
		Vector3 result = {
			v1.x - v2.x,
			v1.y - v2.y,
			v1.z - v2.z
		};
		return result;
	}
	//
	Vector3 Multiply(float scalar, const Vector3& v){
		Vector3 result = {
			scalar * v.x,
			scalar * v.y,
			scalar * v.z
		};
		return result;
    }

    Vector3 Division(const Vector3& v, float scalar) {
		Vector3 result = {
			v.x/scalar,
			v.y/scalar,
			v.z/scalar
		};
		return result;
	}
   
	//
	float Dot(const Vector3& v1, const Vector3& v2){
		
		float result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		return result ;
	}
	//
	float Length(const Vector3& v){
		float length = v.x * v.x + v.y * v.y + v.z * v.z;
		if (length > 0.0f) {
			return sqrtf(length);
		}
		return length;
	}
	//
	Vector3 Normalize(const Vector3& v){
		float length = Length(v);
		if (length > 0.0f) {
			Vector3 result = {
				v.x / length,
				v.y / length,
				v.z / length
			};
			return result;
		}
		Vector3 result = { 0.0f, 0.0f, 0.0f };
		return result;
//
	}