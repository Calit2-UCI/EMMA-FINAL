#include "licensemanagerexports.h"
#include "visagelicensemanager.h"


namespace VisageSDK
{

void VISAGE_DECLSPEC initializeLicenseManager(char *licenseKeyFileFolder)
{
	VisageLicenseManager::initialize(licenseKeyFileFolder);
}

int VISAGE_DECLSPEC checkRegistration(int tid,char *versionString)
{
	VisageLicenseManager *vlm = new VisageLicenseManager();
	int r = vlm->checkRegistration(tid,versionString);
	delete vlm;
	return r;

}

int VISAGE_DECLSPEC checkRegistration(int tid, bool allowPackageLicense, bool silent,char *versionString)
{
	VisageLicenseManager *vlm = new VisageLicenseManager();
	int r = vlm->checkRegistration(tid, allowPackageLicense, silent, versionString);
	delete vlm;
	return r;

}

int VISAGE_DECLSPEC checkRegistration(int tid, char *eds,char *versionString)
{
	VisageLicenseManager *vlm = new VisageLicenseManager();
	int r = vlm->checkRegistration(tid,eds,versionString);
	delete vlm;
	return r;

}

}
