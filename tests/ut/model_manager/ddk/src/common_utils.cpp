#include "common_utils.h"

int32_t readFileFromPath(const string &path, FILE *&fp, uint32_t &fileLength)
{
    if (path.length() == 0)
    {
        printf("ReadFileFromPath error: file path is null.\n");
        return -1;
    }
    // open model file
    char pathTemp[PATH_MAX + 1] = {0x00};
    if (path.length() > PATH_MAX || realpath(path.c_str(), pathTemp) == nullptr)
    {
        printf("ReadFileFromPath error: path size is too long or realpath return nullptr.\n");
        return -1;
    }

    // STATE1
    fp = fopen(pathTemp, "r+");
    if (fp == nullptr)
    {
        printf("ReadFileFromPath open err: %s\n", path.c_str());
        return -1;
    }
    // get model file length
    if (fseek(fp, 0, SEEK_END) != 0)
    {
        printf("ReadFileFromPath fseek SEEK_END error.\n");
        fclose(fp);
        return -1;
    }
    fileLength = ftell(fp);
    if (fseek(fp, 0, SEEK_SET))
    {
        printf("ReadFileFromPath fseek SEEK_SET error.\n");
        fclose(fp);
        return -1;
    }
    if (fileLength <= 0)
    {
        printf("ReadFileFromPath ERROR: fileLength is 0!\n");
        fclose(fp);
        return -1;
    }
    return 0;
}

void readData(const string &path, void **buffer, size_t &size)
{
    FILE *fp = nullptr;
    uint32_t fileLength = 0;
    if (readFileFromPath(path, fp, fileLength) != 0)
    {
        printf("ReadFileFromPath failed\n");
        return;
    }

    void *data = malloc(fileLength);
    if (data == nullptr)
    {
        printf("CreateFromPath ERROR: malloc fail!\n");
        fclose(fp);
        return;
    }

    size = static_cast<uint32_t>(fread(data, 1, fileLength, fp));
    if (size != fileLength)
    {
        printf("CreateFromPath ERROR: size(%zu) != fileLength(%u)\n", size, fileLength);
        free(data);
        data = nullptr;
        fclose(fp);
        return;
    }

    *buffer = data;
    fclose(fp);
}