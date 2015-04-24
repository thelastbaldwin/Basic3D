#version 120

uniform sampler2D tex0;
uniform float opacity;

varying vec2 vUv;

void main(){
    vec4 texColor = texture2D(tex0, vUv);
    if(gl_FragColor.rgb = Vec3(0, 0, 0)){
        discard;
    }else{
        gl_FragColor = vec4(texColor.rgb, opacity);
    }
}