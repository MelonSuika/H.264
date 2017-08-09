#ifndef _MEDIATYPE_H
#define _MEDIATYPE_H
/*音频*/
#define	  MEDIA_TYPE_MP3	              (u8)96  /*mp3 mode 0-4*/
#define   MEDIA_TYPE_G7221C	              (u8)98  /*G722.1.C Siren14*/
#define   MEDIA_TYPE_G719	              (u8)99  /*G719 non-standard of polycom serial 22*/
#define   MEDIA_TYPE_PCMA		          (u8)8   /*G.711 Alaw  mode 5*/
#define   MEDIA_TYPE_PCMU		          (u8)0   /*G.711 ulaw  mode 6*/
#define   MEDIA_TYPE_G721		          (u8)2   /*G.721*/
#define   MEDIA_TYPE_G722		          (u8)9   /*G.722*/
#define	  MEDIA_TYPE_G7231		          (u8)4   /*G.7231*/
#define   MEDIA_TYPE_ADPCM                (u8)5   /*DVI4 ADPCM*/
#define	  MEDIA_TYPE_G728		          (u8)15  /*G.728*/
#define	  MEDIA_TYPE_G729		          (u8)18  /*G.729*/
#define   MEDIA_TYPE_G7221                (u8)13  /*G.7221*/
#define   MEDIA_TYPE_AACLC                (u8)102 /*AAC LC*/
#define   MEDIA_TYPE_AACLD                (u8)103 /*AAC LD*/
#define   MEDIA_TYPE_ISAC_32              (u8)104
#define   MEDIA_TYPE_OPUS                 (u8)120
#define	  MEDIA_TYPE_RED		          (u8)127

/*视频*/
#define   MEDIA_TYPE_MP4	              (u8)97  /*MPEG-4*/
#define   MEDIA_TYPE_H261	              (u8)31  /*H.261*/
#define   MEDIA_TYPE_H262	              (u8)33  /*H.262 (MPEG-2)*/
#define   MEDIA_TYPE_H263	              (u8)34  /*H.263*/
#define   MEDIA_TYPE_H263PLUS             (u8)101 /*H.263+*/
#define   MEDIA_TYPE_H264	              (u8)106 /*H.264*/
#define   MEDIA_TYPE_H264_ForHuawei       (u8)105 /*H.264*/
#define	  MEDIA_TYPE_FEC				  (u8)107 /* fec custom define */
#define   MEDIA_TYPE_H265	              (u8)108 /*H.265*/

/*数据*/
#define   MEDIA_TYPE_H224	              (u8)100  /*H.224 Payload 暂定100*/
#define   MEDIA_TYPE_T120                 (u8)200  /*T.120媒体类型*/
#define   MEDIA_TYPE_H239                 (u8)239  /*H.239数据类型 */
#define   MEDIA_TYPE_MMCU                 (u8)220  /*级联数据通道 */

/*kdv约定的本地发送时使用的活动媒体类型 */
#define   ACTIVE_TYPE_PCMA		          (u8)110   /*G.711 Alaw  mode 5*/
#define   ACTIVE_TYPE_PCMU		          (u8)111   /*G.711 ulaw  mode 6*/
#define   ACTIVE_TYPE_G721		          (u8)112   /*G.721*/
#define   ACTIVE_TYPE_G722		          (u8)113   /*G.722*/
#define	  ACTIVE_TYPE_G7231		          (u8)114   /*G.7231*/
#define	  ACTIVE_TYPE_G728		          (u8)115   /*G.728*/
#define	  ACTIVE_TYPE_G729		          (u8)116   /*G.729*/
#define   ACTIVE_TYPE_G7221               (u8)117   /*G.7221*/
#define   ACTIVE_TYPE_H261	              (u8)118   /*H.261*/
#define   ACTIVE_TYPE_H262	              (u8)119   /*H.262 (MPEG-2)*/
#define   ACTIVE_TYPE_H263	              (u8)120   /*H.263*/
#define   ACTIVE_TYPE_G7221C			  (u8)121	/*G7221c*/
#define   ACTIVE_TYPE_ADPCM				  (u8)122	/*ADPCM*/	
#define   ACTIVE_TYPE_G719                (u8)123   /*G.719*/
#define   ACTIVE_TYPE_OPUS                (u8)127	/*OPUS*/

#endif
