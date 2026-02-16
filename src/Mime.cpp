#include "../includes/Webserver.hpp"

void Mime::initMime(void) {
  mime_types_["html"] = "text/html";
  mime_types_["htm"] = "text/html";
  mime_types_["css"] = "text/css";
  mime_types_["txt"] = "text/plain";
  mime_types_["csv"] = "text/csv";
  mime_types_["md"] = "text/markdown";

  mime_types_["ico"] = "image/vnd.microsoft.icon";
  mime_types_["bmp"] = "image/bmp";
  mime_types_["gif"] = "image/gif";
  mime_types_["png"] = "image/png";
  mime_types_["jpeg"] = "image/jpeg";
  mime_types_["webp"] = "image/jpeg";

  mime_types_["avi"] = "video/x-msvideo";

  mime_types_["gz"] = "application/gzip";
  mime_types_["tar"] = "application/x-tar";
  mime_types_["zip"] = "application/zip";
  mime_types_["doc"] = "application/msword";
  mime_types_["pdf"] = "application/pdf";
  mime_types_["json"] = "application/json";
  mime_types_["bin"] = "application/octet-stream";
  mime_types_["php"] = "application/x-httpd-php";
  mime_types_["sh"] = "application/x-sh";
  mime_types_["xml"] = "application/xml";

  mime_types_["mp3"] = "audio/mp3";

  mime_types_["default"] = "text/html";
}

Mime &Mime::getMimeInfo(void) {
  static Mime mime;

  return (mime);
}

std::string Mime::getMimeType(std::string const &extension) {
  if (mime_types_.find(extension) != mime_types_.end())
    return (mime_types_[extension]);

  return (mime_types_["default"]);
}

std::string Mime::getExtension(std::string const &type) {
  for (iterator it = mime_types_.begin(); it != mime_types_.end(); it++) {
    if (it->second == type)
      return (it->first);
  }

  return (NULL);
}
