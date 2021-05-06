cbuffer CONSTANT_BUFFER : register(b6)
{
	float glow_extraction_threshold;
	float blur_convolution_intensity;
	int2 options;
}

const static uint number_of_downsampled = 6;