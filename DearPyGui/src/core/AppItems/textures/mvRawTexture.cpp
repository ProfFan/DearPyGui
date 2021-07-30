#include "mvRawTexture.h"
#include "mvLog.h"
#include "mvItemRegistry.h"
#include "mvPythonExceptions.h"
#include "mvPythonTranslator.h"
#include "mvUtilities.h"
#include "mvGlobalIntepreterLock.h"

namespace Marvel {

	void mvRawTexture::InsertParser(std::map<std::string, mvPythonParser>* parsers)
	{

		mvPythonParser parser(mvPyDataType::UUID, "Undocumented function", { "Textures", "Widgets" });
		mvAppItem::AddCommonArgs(parser, (CommonParserArgs)(
			MV_PARSER_ARG_ID)
		);

		parser.addArg<mvPyDataType::Integer>("width");
		parser.addArg<mvPyDataType::Integer>("height");
		parser.addArg<mvPyDataType::FloatList>("default_value");
		parser.addArg<mvPyDataType::Integer>("format", mvArgType::KEYWORD_ARG, "internal_dpg.mvFormat_Float_rgba", "Data format.");
		parser.addArg<mvPyDataType::UUID>("parent", mvArgType::KEYWORD_ARG, "internal_dpg.mvReservedUUID_2", "Parent to add this item to. (runtime adding)");
		parser.finalize();

		parsers->insert({ s_command, parser });

		{
			mvPythonParser parser(mvPyDataType::UUID);
			parser.addArg<mvPyDataType::UUID>("item");
			parser.finalize();
			parsers->insert({ "get_raw_texture", parser });
		}

		{
			mvPythonParser parser(mvPyDataType::UUID);
			parser.addArg<mvPyDataType::UUID>("item");
			parser.addArg<mvPyDataType::Bool>("value");
			parser.finalize();
			parsers->insert({ "set_update_enable", parser });
		}
	}

	mvRawTexture::mvRawTexture(mvUUID uuid)
		:
		mvAppItem(uuid)
	{

	}


	PyObject* mvRawTexture::getPyValue()
	{
		return GetPyNone();
	}

	void mvRawTexture::setPyValue(PyObject* value)
	{

		if (value == nullptr)
			return;

		if (PyObject_CheckBuffer(value))
		{
			Py_buffer buffer_info;

			if (!PyObject_GetBuffer(value, &buffer_info,
				PyBUF_CONTIG_RO | PyBUF_FORMAT))
			{
				_value = buffer_info.buf;
				if (_value == nullptr)
				{
					mvThrowPythonError(mvErrorCode::mvTextureNotFound, s_command, "Texture data not valid", this);
				}
			}
			PyBuffer_Release(&buffer_info);
			if (_buffer)
				Py_XDECREF(_buffer);
			Py_XINCREF(value);
			_buffer = value;
		}
	}

	mvRawTexture::~mvRawTexture()
	{
		FreeTexture(_texture);
		
		mvGlobalIntepreterLock gil;
		Py_XDECREF(_buffer);
	}


	bool mvRawTexture::isParentCompatible(mvAppItemType type)
	{
		if (type == mvAppItemType::mvStagingContainer) return true;
		if (type == mvAppItemType::mvTextureRegistry) return true;

		mvThrowPythonError(mvErrorCode::mvIncompatibleParent, s_command,
			"Incompatible parent. Acceptable parents include: mvTextureRegistry, mvStagingContainer.", this);

		MV_ITEM_REGISTRY_ERROR("Drawing item parent must be a drawing.");
		assert(false);
		return false;
	}

	void mvRawTexture::draw(ImDrawList* drawlist, float x, float y)
	{
		if (_dirty)
		{

			if (_value == nullptr)
				return;

			if(_componentType == ComponentType::MV_FLOAT_COMPONENT && _update)
				_texture = LoadTextureFromArrayRaw(_width, _height, (float*)_value, _components);

			if (_texture == nullptr)
				_state.setOk(false);

			_dirty = false;
			return;
		}

		if (_componentType == ComponentType::MV_FLOAT_COMPONENT)
			UpdateRawTexture(_texture, _width, _height, (float*)_value, _components);

	}

	void mvRawTexture::handleSpecificRequiredArgs(PyObject* dict)
	{
		if (!mvApp::GetApp()->getParsers()[s_command].verifyRequiredArguments(dict))
		{
			mvThrowPythonError(mvErrorCode::mvTextureNotFound, s_command, "Texture data not valid", this);
			return;
		}

		for (int i = 0; i < PyTuple_Size(dict); i++)
		{
			PyObject* item = PyTuple_GetItem(dict, i);
			switch (i)
			{
			case 0:
				_width = ToInt(item);
				break;

			case 1:
				_height = ToInt(item);
				break;

			case 2:
				setPyValue(item);
				break;

			default:
				break;
			}
		}
	}

	void mvRawTexture::handleSpecificKeywordArgs(PyObject* dict)
	{
		if (dict == nullptr)
			return;

		if (PyObject* item = PyDict_GetItemString(dict, "format"))
		{
			int format = ToInt(item);
			if (format == 0)
			{
				_components = 4;
				_componentType = mvRawTexture::ComponentType::MV_FLOAT_COMPONENT;
			}
			else if (format == 1)
			{
				_components = 3;
				_componentType = mvRawTexture::ComponentType::MV_FLOAT_COMPONENT;
			}
		}
	}

	void mvRawTexture::getSpecificConfiguration(PyObject* dict)
	{
		if (dict == nullptr)
			return;
	}

	PyObject* mvRawTexture::get_raw_texture(PyObject* self, PyObject* args, PyObject* kwargs)
	{

		mvUUID item;

		if (!(mvApp::GetApp()->getParsers())["get_raw_texture"].parse(args, kwargs, __FUNCTION__,
			&item))
			return GetPyNone();

		if (!mvApp::s_manualMutexControl) std::lock_guard<std::mutex> lk(mvApp::s_mutex);

		auto raw_texture = mvApp::GetApp()->getItemRegistry().getItem(item);
		if (raw_texture == nullptr)
		{
			mvThrowPythonError(mvErrorCode::mvItemNotFound, "get_raw_texture",
				"Item not found: " + std::to_string(item), nullptr);
			return GetPyNone();
		}

		if (raw_texture->getType() == mvAppItemType::mvRawTexture)
		{

			auto pRawTexture = static_cast<mvRawTexture*>(raw_texture);
			auto texture_id = (unsigned int)(size_t)pRawTexture->_texture;
			return Py_BuildValue("k", texture_id);
		}
		else
		{
			mvThrowPythonError(mvErrorCode::mvIncompatibleType, "get_raw_texture",
				"Incompatible type. Expected types include: mvRawTexture", raw_texture);
		}

		return GetPyNone();
	}

	PyObject* mvRawTexture::set_update_enable(PyObject* self, PyObject* args, PyObject* kwargs)
	{

		mvUUID item;
		bool value;

		if (!(mvApp::GetApp()->getParsers())["set_update_enable"].parse(args, kwargs, __FUNCTION__,
			&item, &value))
			return GetPyNone();

		if (!mvApp::s_manualMutexControl) std::lock_guard<std::mutex> lk(mvApp::s_mutex);

		auto raw_texture = mvApp::GetApp()->getItemRegistry().getItem(item);
		if (raw_texture == nullptr)
		{
			mvThrowPythonError(mvErrorCode::mvItemNotFound, "set_update_enable",
				"Item not found: " + std::to_string(item), nullptr);
			return GetPyNone();
		}

		if (raw_texture->getType() == mvAppItemType::mvRawTexture)
		{

			auto pTexture = static_cast<mvRawTexture*>(raw_texture);

			pTexture->_update = value;
		}
		else
		{
			mvThrowPythonError(mvErrorCode::mvIncompatibleType, "set_update_enable",
				"Incompatible type. Expected types include: mvRawTexture", raw_texture);
		}
		
		return GetPyNone();
	}
}