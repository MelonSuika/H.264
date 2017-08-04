void Framing()
{
	FILE* fp = fopen("./h264.data", "rb");
	if( fp == 0 )
		return;

	fseek( fp, 0, SEEK_END );
	int nLen = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	unsigned char * pdata = new unsigned char[nLen];
	fread( pdata, 1, nLen, fp );
	fclose(fp);


	int nCurNalType, nNalType, nPrevFrameStartPos=0;
	//find sps pps 
	int i=0;
	for( ; i<nLen-5; i++ )
		if( pdata[i] == 0 && pdata[i+1] == 0 && pdata[i+2] == 0 && pdata[i+3] == 1 )
		{
			nNalType = pdata[i+4] & 0x1f;
			if( nNalType == 7 || nNalType == 8 )
			{
				//judge slice type
				nCurNalType = nNalType;
				nPrevFrameStartPos = i;
				break;
			}
		}

		FILE* fwsequenc = fopen( "./frame.txt", "w" );

		for( ++i; i<nLen; i++ )
		{
			if( pdata[i] == 0 && pdata[i+1] == 0 && pdata[i+2] == 0 && pdata[i+3] == 1 )
			{
				nNalType = pdata[i+4] & 0x1f;
				if( nNalType == nCurNalType )//连续相同类型的帧
				{
					//不分片slice/A分片/IDR分片   
					if( nCurNalType == 1 || nCurNalType == 2 || nCurNalType == 5 )
					{
						if( pdata[i+5] & 0x80 )//判断first_mb_in_sli
ce是否为0(0 ...N)哥伦布算法
						{
							fprintf( fwsequenc, "got a %d frame, size:%d, startpos:%x\n", nNalType, i-nPrevFrameStartPos, nPrevFrameStartPos );
							nPrevFrameStartPos = i;
						}
					}
					else//B/C/D分片 判断slice id 暂未实现
					{

					}
				}
				else//new frame start
				{
					fprintf( fwsequenc, "got a %d frame, size:%d, star
tpos:%x\n", nNalType, i-nPrevFrameStartPos, nPrevFrameStartPos );
					nPrevFrameStartPos = i;
					nCurNalType = nNalType;
				}
			}

		}
		fclose( fwsequenc );
}
