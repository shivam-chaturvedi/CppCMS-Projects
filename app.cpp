#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<cppcms/application.h>
#include<cppcms/http_response.h>
#include<cppcms/url_dispatcher.h>
#include<cppcms/http_request.h>
#include<cppcms/service.h>
#include<cppcms/applications_pool.h>
#include<cppcms/json.h>
#include<cppcms/serialization.h>
#include<chrono>
#include <boost/date_time/gregorian/gregorian.hpp>
#include<cppcms/http_file.h>


using namespace std;

class App :public cppcms::application{
    private:
    string error_path="error.html";
    string default_error_str="<h1>404 - Page Not Found<br> <span style=\"color:red;text-wrap:wrap;\">To edit defualt error page make a error.html file inside templates folder parent directory</span></h1>";
    
    string render(string path,bool binary_mode=false){
        ios_base::openmode mode=binary_mode?ios::binary|ios::in:ios::in;
        fstream file(path,mode);
        if(!file.is_open()){
            throw "File Not Found !";
        }
        stringstream ss;
        ss<<file.rdbuf();
        file.close();
        return ss.str();
    }

    void serve_static(string url){
        try{
            if(url.find(".html")!=url.npos){
                response().status(200);
                response().content_type("text/html");
                response().out()<<render(url);
            }else if(url.find(".css")!=url.npos){
                response().status(200);
                response().content_type("text/css");
                response().out()<<render(url);
            }else if(url.find(".js")!=url.npos){
                response().status(200);
                response().content_type("application/javascript");
                response().out()<<render(url);
            }else if(url.find(".png")!=url.npos){
                response().status(200);
                response().content_type("image/png");
                response().out()<<render(url,true);
            }else if(url.find(".jpg")!=url.npos){
                response().status(200);
                response().content_type("image/jpg");
                response().out()<<render(url,true);
            }else if(url.find(".gif")!=url.npos){
                response().status(200);
                response().content_type("image/gif");
                response().out()<<render(url,true);
            }else{
                response().status(200);
                response().content_type("application/octet-stream");
                response().out()<<render(url,true);
            }
        }
        catch(...){
            response().status(404);
            response().out()<<default_error_str;
        }
    }

    void send_file(string path){
        try{
            fstream file(path,ios::in|ios::binary);
            int dot=path.find_last_of(".");
            string extension="";
            if(dot!=path.npos){
                extension=path.substr(++dot);
            }
            string name="file";
            int i=path.find("/");
            if(i!=path.npos){
                name=path.substr(++i);
            }
            else{
                name=path;
            }
            response().content_type("application/"+extension);
            response().status(200);
            response().set_header("Content-Disposition","attachment;filename=\" "+name+"\" ");
            response().out()<<file.rdbuf();
            file.close();
        }
        catch(const exception &e){
            response().status(404);
            response().out()<<e.what();
        }
    }

    public:

    App(cppcms::service &srv):cppcms::application(srv){
        // register your url dispatcher here
        dispatcher().assign("/",&App::Home,this);
        dispatcher().assign("/download",&App::download,this);
        dispatcher().assign("/upload",&App::upload,this);
        dispatcher().assign("/css/(.+)",&App::css_handler,this,1);
        dispatcher().assign(".*",&App::not_found,this);
    }

    void upload(){
       if (request().request_method() == "POST") {
            try {
                shared_ptr<cppcms::http::file> file(request().files().front());
                std::string filename = file->filename();
                cout<<endl<<filename<<endl;
                istream &is=file->data();
                ofstream ofs("uploads/"+filename,ios::binary);
                ofs<<is.rdbuf();
                ofs.close();
                // Respond with a success message
                response().out() << "{\"message\": \"File uploaded successfully\", \"filename\": \"" << filename << "\"}";
                response().content_type("application/json");
            } catch (const std::exception &e) {
                response().status(500);
                response().out() << "{\"error\": \"File upload failed\"}";
                response().content_type("application/json");
                std::cerr << "Error handling file upload: " << e.what() << std::endl;
            }
        }else if(request().request_method()=="GET"){
            serve_static("templates/file.html");
        } 
        else {

            response().status(405); // Method not allowed
        }
    }

    void Home(){
        string method=request().request_method();
        
        if(method=="GET"){
            serve_static("templates/index.html");
        }
        else if(method=="POST"){
            std::pair<void*,int> data=request().raw_post_data();
            char * str=static_cast<char*>(data.first);
            string s(str,data.second);
            cout<<endl<<s<<endl;
            stringstream ss(s);
            cppcms::json::value json;
            try{
            json.load(ss,true);
            }
            catch(const exception &e){
                cout<<endl<<e.what()<<endl;
            }
            int age=json.get<int>("age");
            string name=json.get<string>("name");
            string date=json.get<string>("dob");
            boost::gregorian::date Date=boost::gregorian::from_simple_string(date);

            json["age"]=age;
            json["name"]=name;
            boost::gregorian::greg_weekday day_of_week=Date.day_of_week();
            std::string day_of_week_str;

            switch (day_of_week) {
                case boost::gregorian::Monday:    day_of_week_str = "Monday";    break;
                case boost::gregorian::Tuesday:   day_of_week_str = "Tuesday";   break;
                case boost::gregorian::Wednesday: day_of_week_str = "Wednesday"; break;
                case boost::gregorian::Thursday:  day_of_week_str = "Thursday";  break;
                case boost::gregorian::Friday:    day_of_week_str = "Friday";    break;
                case boost::gregorian::Saturday:  day_of_week_str = "Saturday";  break;
                case boost::gregorian::Sunday:    day_of_week_str = "Sunday";    break;
                default: day_of_week_str = "Unknown"; break;
            }
            json["dob"]=day_of_week_str;
            response().status(203);
            response().content_type("application/json");
            response().out()<<json;
        }
    }

    void download(){
        send_file("i.exe");
    }

    void css_handler(string css_file_name){
        serve_static("templates/css/"+css_file_name);
    }

    void not_found(){
        serve_static("templates/error.html");
    }

    virtual void main(string url){
        dispatcher().dispatch(url);
    }
    
};

int main(int argc,char *argv[]){
    try{
        cppcms::service service(argc,argv);
        service.applications_pool().mount(cppcms::applications_factory<App>());
        string ip=service.settings().get<string>("service.ip");
        int port=service.settings().get<int>("service.port");
        cout<<endl<<"Server started on "<<ip<<":"<<port<<endl;
        service.run();
    }catch(const exception &e){
        cout<<endl<<e.what()<<endl;
    }
    return 0;
}


