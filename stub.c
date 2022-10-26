// TODO: actually free whatever memory we're using per request, this leaks because I haven't read about JVM ownership
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcgi_stdio.h>

#define ENV_COUNT 20

const char* ENV_VARS[ENV_COUNT] = {
  "DOCUMENT_ROOT",
  "HTTP_COOKIE",
  "HTTP_HOST",
  "HTTP_REFERER",
  "HTTP_USER_AGENT",
  "HTTPS",
  "PATH",
  "QUERY_STRING",
  "REMOTE_ADDR",
  "REMOTE_HOST",
  "REMOTE_PORT",
  "REMOTE_USER",
  "REQUEST_METHOD",
  "REQUEST_URI",
  "SCRIPT_FILENAME",
  "SCRIPT_NAME",
  "SERVER_ADMIN",
  "SERVER_NAME",
  "SERVER_PORT",
  "SERVER_SOFTWARE"
};

jobject create_map(JNIEnv* env) {
  // instantiate a HashMap
  jclass hmap = (*env)->FindClass(env, "java/util/HashMap");
  jmethodID init = (*env)->GetMethodID(env, hmap, "<init>", "()V");
  jobject map = (*env)->NewObject(env, hmap, init);
  
  // locate the puts method (types erased)
  jmethodID put = (*env)->GetMethodID(env, hmap, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

  // populate the hash map with env variables from fastcgi/lighttpd
  for (int i = 0; i < ENV_COUNT; i++) {
    const char* key = ENV_VARS[i];
    char* value;
    if ((value = getenv(key)))
      (*env)->CallObjectMethod(env, map, put, (*env)->NewStringUTF(env, key), (*env)->NewStringUTF(env, value));
  }

  return map;
}

int main(void) {
  JavaVM* vm;
  JNIEnv* env;
  JavaVMInitArgs args;
  args.nOptions = 0;
  args.version = JNI_VERSION_1_8;

  if (JNI_CreateJavaVM(&vm, (void**) &env, &args) != JNI_OK) {
    return EXIT_FAILURE;
  }

  // load request handler
  jclass cls = (*env)->FindClass(env, "Server"); 
  jmethodID go = (*env)->GetStaticMethodID(env, cls, "go", "(Ljava/util/Map;)Ljava/lang/String;");

  while (FCGI_Accept() >= 0) {
    jstring ret = (*env)->CallStaticObjectMethod(env, cls, go, create_map(env));
    printf("Content-type: text/html\r\n\r\n");
    printf("<!doctype html><html lang=\"en\">");
    printf("<body>");
    printf("%s", (*env)->GetStringUTFChars(env, ret, NULL));
    printf("</body>");
    printf("</html>");
  }
}

