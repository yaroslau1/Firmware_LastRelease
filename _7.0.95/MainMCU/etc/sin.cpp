int TGui::GetSinData( void )
{
	static int temp;
	static float Y;
	Y += 0.4;
	temp = (int)( sin( Y ) * 2000 );
//	if( temp < 0 ) 
//		temp |= 1<<23;
	return temp;
}