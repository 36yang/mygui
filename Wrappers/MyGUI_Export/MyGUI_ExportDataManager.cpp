/*!
	@file
	@author		Albert Semenov
	@date		08/2012
*/

#include "MyGUI_ExportDataManager.h"
#include "MyGUI_ExportDiagnostic.h"
#include "MyGUI_DataFileStream.h"
#include "ExportDefine.h"
#include "ExportMarshaling.h"
#include "MyGUI_DataMemoryStream.h"

namespace Export
{
	namespace ScopeDataManager_IsDataExist
	{
		typedef Convert< bool >::Type (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateIsDataExist( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateIsDataExist");
		}
	}
	namespace ScopeDataManager_GetDataPath
	{
		typedef Convert< const std::string& >::Type (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetDataPath( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetDataPath");
		}
	}
	namespace ScopeDataManager_GetDataSize
	{
		typedef Convert< size_t >::Type (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetDataSize( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetDataSize");
		}
	}
	namespace ScopeDataManager_GetData
	{
		typedef void (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type,
			Convert< void*& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetData( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetData");
		}
	}
	namespace ScopeDataManager_DestroyData
	{
		typedef void (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateDestroyData( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateDestroyData");
		}
	}
	namespace ScopeDataManager_GetDataListSize
	{
		typedef Convert< size_t >::Type (MYGUICALLBACK *ExportHandle)(
			Convert< const std::string& >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetDataListSize( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetDataListSize");
		}
	}
	namespace ScopeDataManager_GetDataListItem
	{
		typedef Convert< const std::string& >::Type (MYGUICALLBACK *ExportHandle)(
			Convert< size_t >::Type );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetDataListItem( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetDataListItem");
		}
	}
	namespace ScopeDataManager_GetDataListComplete
	{
		typedef void (MYGUICALLBACK *ExportHandle)( );
		ExportHandle mExportHandle = nullptr;
		
		MYGUIEXPORT void MYGUICALL ExportDataManager_DelegateGetDataListComplete( ExportHandle _delegate )
		{
			mExportHandle = _delegate;
			MYGUI_PLATFORM_LOG(Info, "ExportDataManager_DelegateGetDataListComplete");
		}
	}
}

namespace MyGUI
{

	ExportDataManager::ExportDataManager()
	{
	}

	void ExportDataManager::initialise()
	{
		MYGUI_PLATFORM_LOG(Info, "* Initialise: " << getClassTypeName());

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully initialized");
	}

	void ExportDataManager::shutdown()
	{
		MYGUI_PLATFORM_LOG(Info, "* Shutdown: " << getClassTypeName());

		MYGUI_PLATFORM_LOG(Info, getClassTypeName() << " successfully shutdown");
	}

	IDataStream* ExportDataManager::getData(const std::string& _name)
	{
		MYGUI_PLATFORM_LOG(Info, _name);

		MapData::iterator item = mDatas.find(_name);
		if (item != mDatas.end())
		{
			(*item).second.second ++;
			return (*item).second.first;
		}

		size_t size = 0;
		if (Export::ScopeDataManager_GetDataSize::mExportHandle != nullptr)
			size = Export::Convert< size_t >::From ( Export::ScopeDataManager_GetDataSize::mExportHandle(Export::Convert< const std::string& >::To(_name)) );

		if (size == 0)
			return nullptr;

		MYGUI_PLATFORM_LOG(Info, size);

		void* data = nullptr;
		if (Export::ScopeDataManager_GetData::mExportHandle != nullptr)
			Export::ScopeDataManager_GetData::mExportHandle(Export::Convert< const std::string& >::To(_name), Export::Convert< void*& >::To(data));

		//MYGUI_PLATFORM_LOG(Info, data);

		if (data == nullptr)
			return nullptr;

		MyGUI::DataMemoryStream* stream = new MyGUI::DataMemoryStream(reinterpret_cast<unsigned char*>(data), size);
		mDatas[_name] = DataCounter(stream, 1);

		/*MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[0]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[1]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[2]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[3]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[4]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[5]);
		MYGUI_PLATFORM_LOG(Info, ((unsigned char*)data)[6]);

		std::stringstream stream((char*)data);

		std::string line;
		while (!stream.eof())
		{
			line.clear();
			std::getline(stream, line, '\n');

			MYGUI_PLATFORM_LOG(Info, line);
		}*/

		return stream;
	}

	void ExportDataManager::destroyData(IDataStream* _data)
	{
		if (_data == nullptr)
			return;

		std::string name;

		for (MapData::iterator item = mDatas.begin(); item != mDatas.end(); item ++)
		{
			if ((*item).second.first == _data)
			{
				if ((*item).second.second > 1)
				{
					(*item).second.second --;
					return;
				}
				else
				{
					name = (*item).first;
					mDatas.erase(item);
					break;
				}
			}
		}

		delete _data;

		if (Export::ScopeDataManager_DestroyData::mExportHandle != nullptr)
			Export::ScopeDataManager_DestroyData::mExportHandle(Export::Convert< const std::string& >::To(name));
	}

	bool ExportDataManager::isDataExist(const std::string& _name)
	{
		if (Export::ScopeDataManager_IsDataExist::mExportHandle != nullptr)
			return Export::Convert< bool >::From ( Export::ScopeDataManager_IsDataExist::mExportHandle(Export::Convert< const std::string& >::To(_name)) );
		return false;
	}

	const VectorString& ExportDataManager::getDataListNames(const std::string& _pattern)
	{
		static VectorString result;
		result.clear();

		size_t count = 0;
		if (Export::ScopeDataManager_GetDataListSize::mExportHandle != nullptr)
			count =  Export::Convert< size_t >::From ( Export::ScopeDataManager_GetDataListSize::mExportHandle(Export::Convert< const std::string& >::To(_pattern)) );

		if (Export::ScopeDataManager_GetDataListItem::mExportHandle != nullptr)
		{
			for (size_t index = 0; index < count; index ++)
				result.push_back(Export::Convert< const std::string& >::From ( Export::ScopeDataManager_GetDataListItem::mExportHandle(Export::Convert< size_t >::To(index)) ));
		}

		if (Export::ScopeDataManager_GetDataListComplete::mExportHandle != nullptr)
			Export::ScopeDataManager_GetDataListComplete::mExportHandle();

		return result;
	}

	const std::string& ExportDataManager::getDataPath(const std::string& _name)
	{
		static std::string result;

		if (Export::ScopeDataManager_GetDataPath::mExportHandle != nullptr)
			result = Export::Convert< const std::string& >::From ( Export::ScopeDataManager_GetDataPath::mExportHandle(Export::Convert< const std::string& >::To(_name)) );

		return result;
	}

} // namespace MyGUI