#ifndef ASIO9_HTTPPARSE_HPP_
#define ASIO9_HTTPPARSE_HPP_

#include <vector>
#include <string_view>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/utility/string_view.hpp>

namespace asio9 {
	static std::pair<std::string_view, std::string_view> parse_url(const std::string_view& url) {
		if (url.size() == 0)
			return { std::string_view(), std::string_view() };
		auto index = url.find("?");
		if (index == std::string_view::npos)
			return { url, std::string_view() };
		else
			return { url.substr(0, index), url.substr(index + 1, url.size() - index - 1) };
	}

	static inline std::pair<std::string_view, std::string_view> parse_url(const boost::string_view& url) {
		return parse_url(std::string_view{ url.data(), url.size() });
	}

	//Split std::string_view
	//From Web:https://stackoverflow.com/questions/48012539/idiomatically-split-a-string-view
	std::vector<std::string_view> Split(const std::string_view str, const char delim = ',')
	{
		std::vector<std::string_view> result;

		int indexCommaToLeftOfColumn = 0;
		int indexCommaToRightOfColumn = -1;

		for (int i = 0; i < static_cast<int>(str.size()); i++)
		{
			if (str[i] == delim)
			{
				indexCommaToLeftOfColumn = indexCommaToRightOfColumn;
				indexCommaToRightOfColumn = i;
				int index = indexCommaToLeftOfColumn + 1;
				int length = indexCommaToRightOfColumn - index;

				// Bounds checking can be omitted as logically, this code can never be invoked 
				// Try it: put a breakpoint here and run the unit tests.
				/*if (index + length >= static_cast<int>(str.size()))
				{
					length--;
				}
				if (length < 0)
				{
					length = 0;
				}*/

				std::string_view column(str.data() + index, length);
				result.push_back(column);
			}
		}
		const std::string_view finalColumn(str.data() + indexCommaToRightOfColumn + 1, str.size() - indexCommaToRightOfColumn - 1);
		result.push_back(finalColumn);
		return result;
	}

	static std::map<std::string_view, std::string_view> parse_urlvar(const std::string_view& url) {
		std::map<std::string_view, std::string_view> map;
		if (url.size() == 0)
			return map;
		auto vResult = Split(url, '&');
		for (auto i = 0; i < vResult.size(); ++i) {
			auto vTemp = Split(vResult.at(i), '=');
			if(vTemp.size() == 2)
				map[std::move(vTemp[0])] = std::move(vTemp[1]);
		}
		return std::move(map);
	}

	static inline std::map<std::string_view, std::string_view> parse_urlvar(const boost::string_view& url) {
		return parse_urlvar(std::string_view(url.data(), url.size()));
	}
}

#endif