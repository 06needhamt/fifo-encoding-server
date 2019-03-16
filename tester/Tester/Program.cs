using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading;

namespace Tester
{
    class Program
    {
        const bool USE_FFMPEG_SAMPLES = false;

        static readonly string BASE_URL = "https://samples.ffmpeg.org/";
        static readonly string FILE_LIST = "allsamples.txt";
        static readonly string SERVER_POST_URL = "http://localhost:5555/namepost";

        static string filesToDownload = string.Empty;
        static string[] fileArray;
        static List<string> testFiles;

        static string linuxInputDirectory = Environment.CurrentDirectory.Replace("D:\\", "/mnt/d/").Replace("\\", "/");
        static string linuxOutputDirectory = string.Format("{0}/{1}", Environment.CurrentDirectory, "outputs").Replace("D:\\", "/mnt/d/").Replace("\\", "/");
        static void Main(string[] args)
        {
            if (Directory.EnumerateFiles(Environment.CurrentDirectory).Count() == 0 && USE_FFMPEG_SAMPLES)
            {
                Console.WriteLine("Downloading Sample Files from samples.ffmpeg.org");
                if (!DoHttpGetRequest(string.Format("{0}{1}", BASE_URL, FILE_LIST), out filesToDownload))
                {
                    Console.WriteLine("Failed to Download Sample Files from samples.ffmpeg.org");
                }
                else
                {
                    fileArray = filesToDownload.Split('\n');
                    fileArray = fileArray.Where(x => x.EndsWith(".mp4")).Select(x => x.TrimStart('.')).ToArray();

                    foreach (string file in fileArray)
                    {
                        int startIdx = file.LastIndexOf('/') + 1;
                        string fileUrl = file.Substring(startIdx, (file.Length - startIdx));
                        string fullUrl = string.Format("{0}{1}", BASE_URL, file);
                        FileInfo outputFile = new FileInfo(string.Format("{0}/{1}", Environment.CurrentDirectory, fileUrl));
                        if (!DoHttpGetRequest(fullUrl, outputFile))
                        {
                            Console.WriteLine(string.Format("Failed to Download Sample File {0} from samples.ffmpeg.org", fileUrl));
                            continue;
                        }
                    }
                }
            }

            testFiles = Directory.EnumerateFiles(Environment.CurrentDirectory).Select(x => Path.GetFileName(x)).ToList();
            if (Directory.Exists(string.Format("{0}/{1}", Environment.CurrentDirectory, "outputs")))
                Directory.Delete(string.Format("{0}/{1}", Environment.CurrentDirectory, "outputs"), true);

            Directory.CreateDirectory(string.Format("{0}/{1}", Environment.CurrentDirectory, "outputs"));

            foreach(string file in testFiles)
            {
                string content;
                if(!DoHttpPostRequest(SERVER_POST_URL, file, out content))
                {
                    Console.WriteLine(string.Format("Error Posting File {0} to server", file));
                    continue;
                }
                Thread.Sleep(15000);
            }

            Console.ReadKey();

        }

        static bool DoHttpGetRequest(string url, out string content)
        {
            content = "";
            var request = (HttpWebRequest)WebRequest.Create(url);
            Console.WriteLine(string.Format("Downloading File List from {0}", url));
            try
            {
                request.Method = "GET";
                request.CookieContainer = null;
                request.Timeout = 25000; // 25 seconds
                var response = (HttpWebResponse)request.GetResponse();
                var streamReader = new StreamReader(response.GetResponseStream());
                content = streamReader.ReadToEnd();
                streamReader.Close();
                Console.WriteLine(string.Format("Successfully Downloaded File List from {0}", url));
                return true;
            }
            catch (WebException ex)
            {
                content = ex.Message;
                return false;
            }
        }
        
        static bool DoHttpGetRequest(string url, FileInfo outputFile)
        {
            byte[] content;
            var request = (HttpWebRequest)WebRequest.Create(url);
            Console.WriteLine(string.Format("Downloading Sample File {0}", url));
            try
            {
                request.Method = "GET";
                request.CookieContainer = null;
                request.Timeout = 25000; // 25 seconds
                var response = (HttpWebResponse)request.GetResponse();
                var binaryReader = new BinaryReader(response.GetResponseStream());
                content = binaryReader.ReadBytes((int) response.ContentLength);
                binaryReader.Close();
                var fileStream = File.Open(outputFile.FullName, FileMode.Create);
                fileStream.Write(content, 0, content.Length);
                fileStream.Flush();
                fileStream.Close();
                Console.WriteLine(string.Format("Successfully Downloaded Sample File {0}", url));
                return true;
            }
            catch (WebException ex)
            {
                content = null;
                return false;
            }
        }

        static bool DoHttpPostRequest(string url, string fileUrl, out string result)
        {
            Console.WriteLine(string.Format("Posting Sample File {0}", fileUrl));
            var httpWebRequest = (HttpWebRequest)WebRequest.Create(url);
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Method = "POST";

            using (var streamWriter = new StreamWriter(httpWebRequest.GetRequestStream()))
            {

                JObject json = JObject.FromObject(new
                {
                    guid = Guid.NewGuid(),
                    command = "Transcode",
                    source = "B",
                    dest = "C",
                    input_file_name = string.Format("{0}/{1}", linuxInputDirectory, fileUrl),
                    output_file_name = string.Format("{0}/{1}", linuxOutputDirectory, fileUrl),
                    item_type = 1,
                    progress = 0
                });

                streamWriter.Write(json.ToString() + "\0");
                streamWriter.Flush();
                streamWriter.Close();
            }
            try
            {
                var httpResponse = (HttpWebResponse)httpWebRequest.GetResponse();
                if (httpResponse != null)
                {
                    using (var streamReader = new StreamReader(httpResponse.GetResponseStream()))
                    {
                        result = streamReader.ReadToEnd();
                        Console.WriteLine(string.Format("Successfully Posted Sample File {0}", fileUrl));
                        return true;
                    }
                }

                result = string.Empty;
                Console.WriteLine(string.Format("Successfully Posted Sample File {0}", fileUrl));
                return true;
            }
            catch (Exception ex)
            {
                result = string.Empty;
                return true;
            }
        }
    }
}
