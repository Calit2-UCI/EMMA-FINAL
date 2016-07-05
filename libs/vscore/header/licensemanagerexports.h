#ifndef __LICENSEMANAGEREXPORTS_H__
#define __LICENSEMANAGEREXPORTS_H__

#ifdef VISAGE_STATIC
	#define VISAGE_DECLSPEC
#else
	#if defined(MAC) || defined(LINUX)
		#define VISAGE_DECLSPEC __attribute__((__visibility__("default")))

	#else
		#ifdef VISAGE_EXPORTS
			#define VISAGE_DECLSPEC __declspec(dllexport)
		#else
			#define VISAGE_DECLSPEC __declspec(dllimport)
		#endif
	#endif

#endif

namespace VisageSDK
{

	void VISAGE_DECLSPEC initializeLicenseManager(char *licenseKeyFileFolder);
	int VISAGE_DECLSPEC checkRegistration(int tid,char *versionString);
	int VISAGE_DECLSPEC checkRegistration(int tid, bool allowPackageLicense, bool silent,char *versionString);
	int VISAGE_DECLSPEC checkRegistration(int tid, char *eds,char *versionString);

}

#endif
