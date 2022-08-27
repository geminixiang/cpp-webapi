#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>


#include "lib/httplib.h"
#include "lib/json.hpp"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

const char *html = R"(
<form id="formElem">
    <input type="file" name="file" accept="image/*">
    <input type="submit">
</form>
<p id="msg"></p>

<script>
formElem.onsubmit = async (e) => {
    e.preventDefault();

    let res = await fetch('/api/image/upload', {
        method: 'POST',
        body: new FormData(formElem)
    });

    msg = document.getElementById("msg");
    msg.innerHTML += await res.text();
};
</script>
)";

void home(const Request& req, Response& resp) {
    time_t my_time = time(NULL);

    printf("%s [%s] %s, %s, %s %i \n", 
            ctime(&my_time),
            req.method.c_str(),
            req.path.c_str(),
            req.remote_addr.c_str(),
            req.version.c_str(),
            req.remote_port);
    resp.set_content(html, "text/html");
    resp.status = 200;
}

void upload_image(const Request& req, Response& resp) {
    auto image_file = req.get_file_value("file");

    cout << "image file length: " << image_file.content.length() << endl
         << "image file name: " << image_file.filename << endl;

    {
        ofstream ofs("upload/" + image_file.filename, ios::binary);
        ofs << image_file.content;
    }

    json result{
        {"name", image_file.filename},
        {"msg", "ok"},
        {"data", image_file.content.length()}};
    resp.set_content(result.dump(), "text/plain");
    resp.status = 200;
}

int main() {

    Server svr;
    svr.Get("/", home);
    svr.Post("/api/image/upload", upload_image);
    svr.listen("0.0.0.0", 8000);
    system("pause");
}