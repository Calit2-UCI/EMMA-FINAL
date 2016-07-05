#ifndef __VISAGELICENSEMANAGER_H__
#define __VISAGELICENSEMANAGER_H__

#include "BigNumber.h"

#define VLTID_SDK_FULL 0
#define VLTID_SDK_VTTS 1
#define VLTID_SDK_LIPSYNC 2
#define VLTID_SDK_VISION_D 3
#define VLTID_SDK_VISION_T 4
#define VLTID_SDK_FACEGEN 5
#define VLTID_SDK_PHOTOFIT 6
#define VLTID_CUSTOM 7

#define NVLTID 8
#define MAXTID 7

#define VLTYPE_EVAL 0
#define VLTYPE_ACAD 1
#define VLTYPE_FULL 2

#define NVLTYPE 3

class VisageLicenseManager 
{
	friend class visageRegistrationManagerDlg;

public:
	static void initialize(char *licenseKeyFileFolder);
	VisageLicenseManager(void);
	char *VisageLicenseManager::getToolName(int tid);
	char *VisageLicenseManager::getLicenseTypeName(int ltid);
	int VisageLicenseManager::checkRegistration(int tid,char *versionString);
	int VisageLicenseManager::checkRegistration(int tid, bool allowPackageLicense, bool silent,char *versionString);
	int VisageLicenseManager::writeLicenseKey(int tid, BigNumber *lk);
	int VisageLicenseManager::addLicenseKey(char *lks);
	int VisageLicenseManager::getLicenseKeyFileData(char *lkf, char *lk, char *eds, int *tid, char *tool, bool *isRedistributable, int &versionMajor, int &versionMinor);
	char *VisageLicenseManager::getMachineID();
	BigNumber *VisageLicenseManager::makeID(char *s);
	int VisageLicenseManager::checkRegistration(int tid, char *eds,char *versionString);
	static int getVisageRootDir(char *dir);
	static int versionNumberFromString(char *versionString,int &versionMajor,int &versionMinor);


private:
	static char visageLicenseKeyNumber[200][30];
	static int nVisageLicenseKeyNumbers;
	static int VisageLicenseManager::getLicenseKeyFromFile(char *lkf, char *lk);
	bool VisageLicenseManager::checkAndConvertDate(BigNumber *ed,char *eds);
	int VisageLicenseManager::checkAndUpdatePriorInstallationFile(char *fn);
	int VisageLicenseManager::checkAndUpdatePriorInstallation();
	BigNumber *VisageLicenseManager::readLicenseKey(int tid);
	int VisageLicenseManager::getLicenseParameters(BigNumber *id, BigNumber *lk, BigNumber *tid, BigNumber *ed, int &versionMajor, int &versionMinor);
	int VisageLicenseManager::getLicenseKeyData(char *lks, char *eds, int *tid, char *tool, bool *isRedistributable, int &versionMajor, int &versionMinor);
	char toolName[200][100];
	int toolPackage[100][200];
	char licenseType[200][100];
	int VisageLicenseManager::checkAndUpdateDate();
};

#endif