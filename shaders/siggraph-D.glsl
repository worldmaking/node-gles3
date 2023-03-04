void mainImage( out vec4 OUT, in vec2 P )
{
    vec4 a = A(P);
    vec4 p = B(P);
    vec2 q = p.xy-P;
    //p = vec4(R/2., sin(iTime), cos(iTime));
    
    float d = max(length(q)-1., 0.);
    
    vec2 v = normalize(a.xy);
    float angle = atan(v.y, v.x);
    vec2 q1 = rotate2(q, angle);
    //d = line2(P, p.xy - s*v, p.xy + s*v);
    
    float size = 12.;
    d = box2(q1, vec2(size));
    //d = orientedBox2(q, vec2(-2.), vec2(2.), atan(v.y, v.x));
    
    //d = abs(d);
    
    vec2 tc = mod(q1/size, 1.);
    
    float divs = float(IMG_GRID);
    
    int cx = int(p.w) % IMG_GRID;
    int cy = int(p.w) / IMG_GRID;

    vec2 ctc = vec2(cx, cy)/divs;
    vec2 tc1 = tc/divs;// + ctc/divs;



    tc1.x += float(cx)/divs;
    tc1.y += float(cy)/divs;
    
    vec3 img = texture(iChannel3, tc1).rgb;
    
    //img.rg += p.wz* 0.25;
    
    float c = clamp(exp(-d * 2.), 0., 1.);
    
    OUT = vec4( img, 1.  )*c;
    
    //OUT.rgb = a.www * hsl2rgb(vec3(0.25*dot(a.xy, vec2(1,0)), abs(a.z), 0.75));
}