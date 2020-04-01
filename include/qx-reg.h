#ifndef QXREG_H
#define QXREG_H
#include "qx.h"
#include "Windows_c++17_compat.h"

namespace Qx
{
//===============================================================================================================
// REG
//===============================================================================================================

class Reg
{
//-Inner Classes------------------------------------------------------------------------------------------------
    class RegOpReport
    {
    //-Class Functions----------------------------------------------------------------------------------------------

    };

//-Class Functions----------------------------------------------------------------------------------------------
public:
    enum RegHive {HKCR, HKCU, HKLM, HKU, HKCC};
    static QString readRegString(RegHive valueHive, const QString &valueKey, const QString &valueName);

private:
    static HKEY resolveHiveEnum(RegHive regHive);
    static LONG getStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue);
    static LONG getDWORDRegKey(HKEY hKey, const std::wstring &strValueName, DWORD &nValue, DWORD nDefaultValue);
    static LONG getBoolRegKey(HKEY hKey, const std::wstring &strValueName, bool &bValue, bool bDefaultValue);
};

}
#endif // REG_H
