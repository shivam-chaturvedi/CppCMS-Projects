#include<cppcms/application.h>
#include<iostream>
#include<cppcms/service.h>
#include<cppcms/applications_pool.h>
#include<cppcms/http_response.h>
#include<fstream>
#include<string>
#include "parse.cpp"

using namespace std;

class App: public cppcms::application{
	private:
	string error_path="error.html";

	string render(string path){

		try{
		fstream file("templates/"+path);
		if(!file.is_open()){
			if(path==error_path){
				return "<h1>Page Not Found !!</h1><br><h2>To Make Your Custom Error Page make a error.html file in templates folder</h2>";
			}
			throw "";
		}
		string s;
		char ch;
		while(file.get(ch)){
			if(ch=='\n'){
				// continue;
			}
			s+=ch;
		}
		return s;
		}
		catch(...){
			return render(error_path);
		}
	}


	public:

	App(cppcms::service &srv):cppcms::application(srv){}

	virtual void main(string url){
		auto urls=parse_json("urls.json");
		string path=urls[url];
		string html=render(path);
		if(path.find(".css")!=path.npos){
			response().content_type("text/css");
		}
		response().out()<<html;
		cout<<endl<<"sending "<<html.size()<<" bytes";
	}

};


int main(int argc ,char **argv){
	try{
	cppcms::service s(argc,argv);
	s.applications_pool().mount(cppcms::applications_factory<App>());
	string ip=s.settings().get<string>("service.ip");
	int port=s.settings().get<int>("service.port");
	
	cout<<endl<<"Server Started on "<<ip<<":"<<port<<endl;
	s.run();
	}
	catch(exception &r){
		cout<<r.what();
	}
	return 0;
}