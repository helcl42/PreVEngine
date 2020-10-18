

// utils -> has to be included(not standalone)
const float PI = 3.1415927;
const float PI_2 = 1.5707963;
const float INV_PI = 0.3183098;
const float INV_PI_2 = 0.636619772;

float Log10(in float x)
{
	return log2(x) / log2(10);
}

vec2 Power(in vec2 b, in float e)
{
	return vec2(pow(b.x, e), pow(b.y, e));
}

vec3 Power(in vec3 b, in float e)
{
	return vec3(pow(b.x, e), pow(b.y, e), pow(b.z, e));
}

vec4 Power(in vec4 b, in float e)
{
	return vec4(pow(b.x, e), pow(b.y, e), pow(b.z, e), pow(b.w, e));
}

float SquareLength(in vec2 v)
{
	return v.x * v.x + v.y * v.y;
}

float SquareLength(in vec3 v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float SquareLength(in vec4 v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

vec2 VecMax(in vec2 a, in vec2 b)
{
	return SquareLength(a) > SquareLength(b) ? a : b;
}

vec3 VecMax(in vec3 a, in vec3 b)
{
	return SquareLength(a) > SquareLength(b) ? a : b;
}

vec4 VecMax(in vec4 a, in vec4 b)
{
	return SquareLength(a) > SquareLength(b) ? a : b;
}

float Luminance(in vec3 rgb)
{
	const vec3 kLum = vec3(0.2126, 0.7152, 0.0722);
	return max(dot(rgb, kLum), 0.0001); // prevent zero result
}

float LinearizeDepth(in float depth, in float znear, in float zfar)
{
	float zn = depth * 2.0 - 1.0; // convert back to ndc
	return 2.0 * znear * zfar / (zfar + znear - (zfar - znear) * zn);
}

float Median(in float r, in float g, in float b)
{
	return max(min(r, g), min(max(r, g), b));
}
