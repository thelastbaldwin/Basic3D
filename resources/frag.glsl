uniform sampler2D tex0;
uniform float opacity;

void main(){
    gl_FragColor = vec4(1.0, 1.0, 1.0, opacity);
}