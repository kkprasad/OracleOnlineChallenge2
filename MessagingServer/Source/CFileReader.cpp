#include "CFileReader.h"

long CFileReader::GetPrivateProfileString(const char *a_spsection,const char *a_spkey,const char *a_spdefaultString, char *a_spdata,const char *a_spfileName)
{
	if((a_spsection == NULL) || (a_spkey == NULL) || (a_spdefaultString == NULL) || (a_spfileName ==NULL) || \
			(a_spdata == NULL))
		return -1;
	FILE *fp;
	char l_cpsection[100];

	if(strlen(a_spsection) > 96)
	{
		fprintf(stderr,"\nError :: Section Length is more than 96\n\n");
		return -1;
	}
	fp=fopen(a_spfileName,"r");

	if(fp==NULL)
	{
		fprintf(stderr,"Error %d : %s \n",errno,strerror(errno));
		throw(errno);
		return -1;
	}


	l_cpsection[0]='[';
	l_cpsection[1]='\0';
	strcat(l_cpsection,a_spsection);
	strcat(l_cpsection,"]");

	char l_cpdataBuffer[100];
	
	char *l_cptagName;
	char *l_cptagValue;	
	char l_cptempDataBuffer[100];
	unsigned long l_ullength=0;
	int l_ulsectionLength = 0;
	strcpy(a_spdata,a_spdefaultString);


	fgets(l_cpdataBuffer,100,fp);
	while(!feof(fp))
	{
		if(l_cpdataBuffer[0]=='[')
		{
			l_ulsectionLength = strlen(l_cpdataBuffer);

			if(l_cpdataBuffer[l_ulsectionLength - 1] == '\n')
				l_ulsectionLength--;

			if(l_cpdataBuffer[l_ulsectionLength - 1] == '\r')
				l_ulsectionLength--;

			l_cpdataBuffer[l_ulsectionLength] = '\0';

			if(strcmp(l_cpdataBuffer,l_cpsection)==0)
			{
				fgets(l_cpdataBuffer,100,fp);

				while(l_cpdataBuffer[0] != '[')
				{
					strcpy(l_cptempDataBuffer,l_cpdataBuffer);
					l_cptagName = strtok(l_cptempDataBuffer,"=");
					l_cptagValue = strtok(NULL,"=");

					////	while(*a_spdataBuffer == ' ' || *a_spdataBuffer == '\t' || \
					*a_spdataBuffer == '\n')
						////		a_spdataBuffer++;

						////	if(a_spdataBuffer[0] != '\0')
						if(l_cptagName && l_cptagValue)
						{

							if(strcmp(l_cptagName,a_spkey) == 0)
							{
								l_ullength = strlen(l_cptagValue);
								if(l_cptagValue[l_ullength - 1] == '\n')
									l_ullength--;

								if(l_cptagValue[l_ullength - 1] == '\r')
									l_ullength--;

								//	if(l_ullength > (ulL_Size - 1))
								//		l_ullength = ulL_Size -1;
								if(l_ullength != 0)
									strncpy(a_spdata,l_cptagValue,l_ullength);
								a_spdata[l_ullength] = '\0';
								fclose(fp);
								return (l_ullength);
							}
						}
					fgets(l_cpdataBuffer,100,fp);
					if(feof(fp))
						break;
				}
			}
		}
		fgets(l_cpdataBuffer,100,fp);
	}
	fclose(fp);
	return (l_ullength);
}

long CFileReader::GetPrivateProfileSection( const char *a_spsection, char *a_spdata, const char *a_spfileName)
{
	if((a_spsection == NULL) || (a_spfileName ==NULL) || (a_spdata == NULL)) return -1;
	FILE *fp;
	char l_cpsection[100];

	if(strlen(a_spsection) > 96)
	{
		fprintf(stderr,"\nError :: Section Length is more than 96\n\n");
		return -1;
	}
	fp=fopen(a_spfileName,"r");

	if(fp==NULL)
	{
		fprintf(stderr,"Error %d : %s \n",errno,strerror(errno));
		throw(errno);
		return -1;
	}


	l_cpsection[0]='[';
	l_cpsection[1]='\0';
	strcat(l_cpsection,a_spsection);
	strcat(l_cpsection,"]");

	char l_cpdataBuffer[100];
	
	char *l_cptagName;
	char *l_cptagValue;	
	char l_cptempDataBuffer[100];
	unsigned long l_ullength=0;
	int l_ulsectionLength = 0;


	fgets(l_cpdataBuffer,100,fp);
	sprintf( a_spdata, "");
	while(!feof(fp))
	{
		if(l_cpdataBuffer[0]=='[')
		{
			l_ulsectionLength = strlen(l_cpdataBuffer);

			if(l_cpdataBuffer[l_ulsectionLength - 1] == '\n')
				l_ulsectionLength--;

			if(l_cpdataBuffer[l_ulsectionLength - 1] == '\r')
				l_ulsectionLength--;

			l_cpdataBuffer[l_ulsectionLength] = '\0';

			if(strcmp(l_cpdataBuffer,l_cpsection)==0)
			{
				fgets(l_cpdataBuffer,100,fp);
				while(l_cpdataBuffer[0] != '[')
				{
					if(l_cpdataBuffer[l_ulsectionLength - 1] == '\n')
						l_ulsectionLength--;

					if(l_cpdataBuffer[l_ulsectionLength - 1] == '\r')
						l_ulsectionLength--;

					l_ulsectionLength = strlen(l_cpdataBuffer);
					l_cpdataBuffer[l_ulsectionLength] = '\0';
					strcat( a_spdata, l_cpdataBuffer);
					strcat( a_spdata, "\n");
					fgets(l_cpdataBuffer,100,fp);
					if ( feof( fp) ) break;
				}
				break;
			}
		}
		fgets(l_cpdataBuffer,100,fp);
	}
	fclose(fp);
	return (l_ullength);
}


