#include <native/native.hpp>

using namespace native;

//  Windows
#ifdef _WIN32
#include <Windows.h>
double getWallTime(){
    LARGE_INTEGER time,freq;
    if (!QueryPerformanceFrequency(&freq)){
        //  Handle error
        return 0;
    }
    if (!QueryPerformanceCounter(&time)){
        //  Handle error
        return 0;
    }
    return (double)time.QuadPart / freq.QuadPart;
}

double getCpuTime(){
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }else{
        //  Handle error
        return 0;
    }
}

//  Posix/Linux
#else
#include <time.h>
#include <sys/time.h>
double getWallTime(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

double getCpuTime(){
    return (double)clock() / CLOCKS_PER_SEC;
}

#endif

void initGlobalFormats()
{
    static bool initiated = false;
    if(initiated)
    {
        return;
    }

    initiated = true;
    UriTemplateFormat::AddGlobalFormat("formatName1", "(?:[A-Z0-9\\-]{2}|[A-Z]{3})");
    UriTemplateFormat::AddGlobalFormat("FormatName2", "[0-9]{1,4}[A-Z]?");
    UriTemplateFormat::AddGlobalFormat("FormatName3", "[0-9]{2}[A-Z]{3}[0-9]{2}");
    UriTemplateFormat::AddGlobalFormat("FormatComplex", "{format1:formatName1}_{number:FormatName2}/{date:FormatName3}");
    UriTemplateFormat::AddGlobalFormat("FormatName6", "[A-Z]{3}");
    UriTemplateFormat::AddGlobalFormat("FormatName4", "{complex:FormatComplex}/someText/{format5:FormatName6}");
}

int main() {
    std::string reLibName;

    const double startTimeWall = getWallTime();
    const double startTimeCPU = getCpuTime();

    initGlobalFormats();
    const UriTemplate uriTemplate("someText/{someText:FormatName4}/longUri/{format1:formatName1}");
    const std::string uriText("someText/AC_1234/01JAN15/someText/ABC/longUri/AB");
    const int timesParse = 1000000;
    for(int i = 0; i < timesParse; ++i) {
        UriTemplateValue extractedValues;
        uriTemplate.extract(extractedValues, uriText);
    }
    const double elapcedWallTime = getWallTime() - startTimeWall;
    const double elapsedCpuTime = getCpuTime() - startTimeCPU;
    std::cout << native::getRegexLibName() << " UriTemplate::extract of " << timesParse << " times extract. CPU time: " << elapsedCpuTime << ", wall time: " << elapcedWallTime << "\n";
}
