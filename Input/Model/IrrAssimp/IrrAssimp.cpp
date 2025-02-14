#ifdef ASSIMP
#include "IrrAssimp.h"

#include <iostream>

using namespace irr;

IrrAssimp::IrrAssimp(irr::scene::ISceneManager* smgr)
{
    Smgr = smgr;
    Cache = smgr->getMeshCache();
    FileSystem = smgr->getFileSystem();

    Exporter = new IrrAssimpExport();
    Importer = new IrrAssimpImport(smgr);
}

IrrAssimp::~IrrAssimp()
{
    //dtor
    delete Importer;
    delete Exporter;
}

bool IrrAssimp::exportMesh(irr::scene::IMesh* mesh, irr::core::stringc format, irr::core::stringc path)
{
    Exporter->writeFile(mesh, format, path);
}

irr::scene::IAnimatedMesh* IrrAssimp::getMesh(const io::path& path)
{
	scene::IAnimatedMesh* msh = Cache->getMeshByName(path);
	if (msh)
		return msh;

	io::IReadFile* file = FileSystem->createAndOpenFile(path);
	if (!file)
	{
		//os::Printer::log("Could not load mesh, because file could not be opened: ", path, ELL_ERROR);
		return 0;
	}

	if (isLoadable(path))
    {
        msh = Importer->loadMesh(path);

        if (msh)
        {
            Cache->addMesh(path, msh);
            msh->drop();
        }
    }

	file->drop();

    /*
        if (!msh)
            os::Printer::log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
        else
            os::Printer::log("Loaded mesh", filename, ELL_INFORMATION);
    */

	return msh;
}

irr::core::stringc IrrAssimp::getError()
{
    return Importer->Error;
}


core::array<ExportFormat> IrrAssimp::getExportFormats()
{
    core::array<ExportFormat> formats;

    Assimp::Exporter exporter;
    for (int i = 0; i < exporter.GetExportFormatCount(); ++i)
    {
         const aiExportFormatDesc* formatDesc = exporter.GetExportFormatDescription(i);
         formats.push_back(ExportFormat(formatDesc->fileExtension, formatDesc->id, formatDesc->description));
    }

    return formats;
}


bool IrrAssimp::isLoadable(irr::core::stringc path)
{
    Assimp::Importer importer;

    irr::core::stringc extension;
    irr::core::getFileNameExtension(extension, path);
    return importer.IsExtensionSupported (extension.c_str());
}
#endif
