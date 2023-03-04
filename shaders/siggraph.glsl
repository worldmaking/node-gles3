void mainImage( out vec4 OUT, in vec2 P )
{
    vec2 uv = P/R;

    // zoom in:
    //uv = uv * 0.25 + 0.125;

    OUT = texture(iChannel3, uv);
}