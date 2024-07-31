#include<cppcms/application.h>
#include<iostream>
#include<cppcms/service.h>
#include<cppcms/applications_pool.h>
#include<cppcms/http_response.h>
#include<fstream>
#include "parse.cpp";

using namespace std;

class App: public cppcms::application{
	private:
	string error_path="error.html";

	string render(string path){

		try{
		fstream file("templates/"+path);
		if(!file.is_open()){
			cerr<<flush<<"File Not Found !!"<<endl;
			throw "";
		}
		string s;
		char ch;
		while(file.get(ch)){
			s+=ch;
		}
		return s;
		}
		catch(...){
			return render(error_path);
		}
	}


	public:

	App(cppcms::service &srv):cppcms::application(srv){
		cout<<"Server Started on 127.0.0.1:8080"<<endl;
	}

	virtual void main(string url){
			if(url=="/"){
				response().out()<<render("index.html");
			}
			else if(url=="/hello"){
				string urls=render("urls.json");
				
			}
			else{
				response().out()<<render(error_path);
			}
	}

};


int main(int argc ,char **argv){
	cppcms::service s(argc,argv);
	s.applications_pool().mount(cppcms::applications_factory<App>());
	s.run();
	return 0;
}