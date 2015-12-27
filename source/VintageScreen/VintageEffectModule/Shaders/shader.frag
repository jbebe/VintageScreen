#version 120

uniform sampler2D texture;
uniform float time;

const float WIDTH = 768;
const float HEIGHT = 576;

vec3 outputColor = vec3(0,0,0);

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 rand2(vec2 co){
	return vec2(rand(co + 1.732050f), rand(co + 1.414213f));
}

vec3 screenFilter(vec3 a, vec3 b){
	vec3 color;

	color.r = (1.f - ((1.f- a.r) * (1.f- b.r)));
	color.g = (1.f - ((1.f- a.g) * (1.f- b.g)));
	color.b = (1.f - ((1.f- a.b) * (1.f- b.b)));

	return color;
}

vec3 bilinearFilter() {
	float u_ratio = 0.5f;
	float v_ratio = 0.5f;
	float u_opposite = 1.f - u_ratio;
	float v_opposite = 1.f - v_ratio;

	vec2 currentPos = gl_TexCoord[0].xy;
	vec3 col00 = texture2D(texture, currentPos + vec2(      0,        0)).rgb;
	vec3 col01 = texture2D(texture, currentPos + vec2(      0, 1/HEIGHT)).rgb;
	vec3 col10 = texture2D(texture, currentPos + vec2(1/WIDTH,        0)).rgb;
	vec3 col11 = texture2D(texture, currentPos + vec2(1/WIDTH, 1/HEIGHT)).rgb;
	
	return 	(col00 * u_opposite + col01 * u_ratio) * v_opposite + 
			(col10 * u_opposite + col11 * u_ratio) * v_ratio;
}

vec3 randomLineOffset(){
	vec2 currentPos = gl_TexCoord[0].xy;
	vec3 pixel = outputColor;
	
	float offset = rand(vec2(floor((currentPos.y*HEIGHT)/6.f), time))*0.002;

	return texture2D(texture, currentPos + vec2(offset, 0)).rgb;
}

vec3 scanline() {
	vec2 currentPos = gl_TexCoord[0].xy;
	float currentPosY = (gl_FragCoord.y);
	float modY = mod(currentPosY, 4.f);
	vec3 color = outputColor;
	
	if (modY < 1.){
		return color;
	}
	if (modY < 2.){
		return color * 0.7f;
	}
	if (modY < 3.){
		return min(color * 1.2f, vec3(1,1,1));
	}
	return color * 0.7f;
}

vec3 darkCorners() {
	vec2 currentPos = gl_TexCoord[0].xy;
	vec3 pixel = outputColor;
	
	float w = currentPos.x * WIDTH;
	float h = currentPos.y * HEIGHT;

	float half_width = WIDTH/2.;
	float half_height = HEIGHT/2.;
	float dist_x = abs(w - half_width);
	float dist_y = abs(h - half_height);
	float vertical_ratio = (half_width - dist_x) / (half_width);
	float horizontal_ratio = (half_height - dist_y) / (half_height);

	vertical_ratio   = pow(vertical_ratio,   0.6f) + 0.6f;
	horizontal_ratio = pow(horizontal_ratio, 0.6f) + 0.6f;

	vertical_ratio = min(vertical_ratio, 1.f);
	horizontal_ratio = min(horizontal_ratio, 1.f);

	return pixel * vertical_ratio * horizontal_ratio;
}

vec3 retrace() {
	vec2 currentPos = gl_TexCoord[0].xy;
	vec3 pixel = outputColor;
	
	float w = currentPos.x * WIDTH;
	float h = currentPos.y * HEIGHT;

	float counter = mod(time*400, HEIGHT);

	float distance_to_counter = mod((h + counter), HEIGHT);
	float distance_ratio = (HEIGHT - distance_to_counter) / HEIGHT;

	vec3 hsv_pixel = rgb2hsv(pixel);
	hsv_pixel.z = distance_ratio * 0.5;

	return 
		min(
			mix(pixel, hsv2rgb(hsv_pixel), 0.2),
			vec3(1,1,1)
		);
}

vec3 blur(){
    // siman zajjal atlagolj kornyezo pixeleket es megkapjuk a homalyt
	vec2 currentPos = gl_TexCoord[0].xy;

	vec3 tmpOutCol = vec3(0,0,0);

	float size = 4;
	float step = 1.f/(WIDTH/10.f);
	float max = step * size;
	
	vec2 minClamp = vec2(0,0);
	vec2 maxClamp = vec2(1,1);

	vec2 dirLU = vec2(-1, -1);
	vec2 dirCU = vec2( 0, -1);
	vec2 dirRU = vec2( 1, -1);
	vec2 dirLM = vec2(-1,  0);
	vec2 dirRM = vec2( 1,  0);
	vec2 dirLD = vec2(-1,  1);
	vec2 dirCD = vec2( 0,  1);
	vec2 dirRD = vec2( 1,  1);

	for (float i = 1.f/WIDTH; i < max; i += step){
		vec2 r = rand2(vec2(i, i))/70.f;
		vec3 tmpCol;

		vec2 genPosLU = clamp(currentPos + dirLU*r, minClamp, maxClamp);
		vec2 genPosCU = clamp(currentPos + dirCU*r, minClamp, maxClamp);
		vec2 genPosRU = clamp(currentPos + dirRU*r, minClamp, maxClamp);
		vec2 genPosLM = clamp(currentPos + dirLM*r, minClamp, maxClamp);
		vec2 genPosRM = clamp(currentPos + dirRM*r, minClamp, maxClamp);
		vec2 genPosLD = clamp(currentPos + dirLD*r, minClamp, maxClamp);
		vec2 genPosCD = clamp(currentPos + dirCD*r, minClamp, maxClamp);
		vec2 genPosRD = clamp(currentPos + dirRD*r, minClamp, maxClamp);

		tmpCol = texture2D(texture, genPosLU).rgb;
		tmpCol += texture2D(texture, genPosCU).rgb;
		tmpCol += texture2D(texture, genPosRU).rgb;
		tmpCol += texture2D(texture, genPosLM).rgb;
		tmpCol += texture2D(texture, genPosRM).rgb;
		tmpCol += texture2D(texture, genPosLD).rgb;
		tmpCol += texture2D(texture, genPosCD).rgb;
		tmpCol += texture2D(texture, genPosRD).rgb;

		tmpOutCol = mix(tmpOutCol, tmpCol/9.f, 0.5f);
	}

	vec3 avg = vec3(0.2126f*tmpOutCol.r + 0.7152f*tmpOutCol.g + 0.0722f*tmpOutCol.b);
	avg = pow(avg, vec3(3));

	return screenFilter(avg, outputColor);
}

vec3 colorFineTune(){
	vec3 pixelColor = outputColor;

	// make it less colorful
	vec3 hsv_color = rgb2hsv(pixelColor);
	hsv_color.y = min(hsv_color.y * 0.9f, 1);
	pixelColor = hsv2rgb(hsv_color);

	// make it green. my old crt became green too! :D
	pixelColor.g = min(pixelColor.g * 1.2f, 1);

	return pixelColor;
}

void main() {
  
	if (gl_TexCoord[0].x >= 1.f || gl_TexCoord[0].y >= 1.f || gl_TexCoord[0].z >= 1.f){
		float plastic = 1 - distance(gl_FragCoord.xy, vec2(WIDTH/2,HEIGHT/2))/480.f;
		plastic *= 0.3f;
		gl_FragColor = vec4(plastic*1.2f, plastic*1.2f, plastic, 1);
	}
	else {
		outputColor = vec3(1);
		outputColor = bilinearFilter();
		outputColor = randomLineOffset();
		outputColor = scanline(); // SOLVED BUT NOT TEXTURE-WISE
		outputColor = blur(); // TOO SLOW
		outputColor = retrace();
		outputColor = darkCorners();
		outputColor = colorFineTune();
		gl_FragColor = vec4(outputColor, 1);
	}
}