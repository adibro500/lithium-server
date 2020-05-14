
#include <li/sql/mysql.hh>
#include <li/sql/sql_orm.hh>
#include <li/sql/sql_common.hh>
#include <li/sql/type_hashmap.hh>
#include <li/http_backend/http_backend.hh>
#include <li/sql/symbols.hh>
#include <li/sql/mysql_statement.hh>
#include "nlohmann/json.hpp"
#include "symbols.hh"
using json = nlohmann::json;
using namespace li;
int main() {
http_api api;
auto db = li::mysql_database(s::host = "192.168.253.1",
                             s::database = "diary",
                             s::user = "root",
                             s::password = "umadevitilak",
                             s::port = 3306,
                             s::charset = "utf8");
auto posts = li::sql_orm_schema(db, "dailyitems" /* the table name in the SQL db*/)

              .fields(s::id(s::primary_key) = std::string(),
                      s::time = std::string(),
                      s::post = std::string(),
                      s::day = std::string(),
		      s::date = std::string());
json result,builder;
auto sstrings = posts.connect();
api.get("/api/posts") =[&](http_request& request,http_response& resp) {
//auto c = db.connect();
typedef decltype(posts.all_fields()) post;
    std::vector<post> table;

    auto c = posts.connect(request.fiber);
 c.forall([&] (auto f) { 
table.emplace_back(f); });

//c("select id,time,day,date,post from dailyitems").forall([&] (auto& f){
//builder["id"]=it->id;
//builder["time"]=it->time;
//builder["day"]=it->day;
//builder["date"]=it->date;
//builder["post"]=it->post;
//result.push_back(builder);
//std::cout << f->id;
//});
  for(auto& f : table)
    { 
  builder["id"]=f.id;
  builder["time"]=f.time;
 builder["day"]=f.day;
builder["date"]=f.date;
builder["post"]=f.post;
result.push_back(builder);
}
resp.write(result.dump(4));
result.clear();
};
api.post("/api/save/post") = [&] (http_request& request, http_response& response) {
json res;
res["success"]=true; 
auto params = request.post_parameters(s::time = std::string(), s::day = std::string(),s::date = std::string(),s::post = std::string(),s::id = std::string());
  posts.connect().insert(s::time = params.time, s::day = params.day, s::date = params.date, s::post = params.post, s::id = params.id, request);
 response.write(res.dump(4));
};

api.post("/api/edit/post") = [&] (http_request& request, http_response& response) {
json res2;
auto params = request.post_parameters(s::time = std::string(), s::day = std::string(),s::date = std::string(),s::post = std::string(),s::id = std::string());
 res2["updated_id"]=params.id; 
auto u = posts.connect().find_one(s::id = params.id);
u->day = params.day;
u->date = params.date;
u->time = params.time;
u->post = params.post;
posts.connect().update(*u);
response.write(res2.dump(4));
};

api.post("/api/delete/post") = [&] (http_request& request,http_response& response) {
json res2;
auto params = request.post_parameters(s::time = std::string(), s::day = std::string(),s::date = std::string(),s::post = std::string(),s::id = std::string());
 res2["deleted_id"]=params.id; 
auto u = posts.connect().find_one(s::id = params.id);
posts.connect().remove(*u);
response.write(res2.dump(4));
};



http_serve(api, 8000);
}

