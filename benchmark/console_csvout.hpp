#pragma once

class file_and_std_ostream
{
public:
	template<typename mode_type>
	file_and_std_ostream(const std::string& path, mode_type mod) : fstream(path, mod) {};
	template<typename T> file_and_std_ostream& operator<<(const T& v)
	{
		std::cout << v;
		fstream << v;
		return *this;
	}
	typedef std::ostream& (*stream_function)(std::ostream&);
	file_and_std_ostream& operator<<(stream_function func)
	{
		func(std::cout);
		func(fstream);
		return *this;
	}
private:
	std::ofstream fstream;
};

int main(int argc, const char* argv[])
{
	std::vector<std::string> tests = {
		"global table",
		"table chain",
		"c function call",
		"lua function call",
		"C++ object member call",
		"Returning a class object",
		"std random(C++11) bind"
	};


	std::string bin_name = argv[0];
	if (bin_name.find("output_csv_title") != std::string::npos)
	{
		file_and_std_ostream out(std::string("result.csv"), std::ios::out | std::ios::binary);
		out << "Library + " LUA_RELEASE;
		for (auto name : tests)
		{
			out << "," << name;
		}
		out << std::endl;
		return 0;
	}

	file_and_std_ostream out(std::string("result.csv"), std::ios::out | std::ios::app | std::ios::binary);
	benchmark::BenchmarkRunner::instance().execute(tests, out);
}
