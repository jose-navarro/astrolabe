/** \file gp_astrolabe_header_file_parser.cpp
  \brief Implementation file for gp_astrolabe_header_file_parser.hpp
  \ingroup ASTROLABE_metadata
*/

#include "gp_astrolabe_header_file_parser.hpp"

void
gp_astrolabe_header_file_parser::
errors_add
(string& the_error)
{
  {
    list_of_errors_.push_back(the_error);
  }
}

int
gp_astrolabe_header_file_parser::
errors_dim
(void)
const
{
  {
    return (int)list_of_errors_.size();
  }
}

string
gp_astrolabe_header_file_parser::
errors_get
(int at_position)
{
  set_precondition (errors_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < errors_dim());

  string result;

  {
    result = list_of_errors_[at_position];
  }

  return result;
}

bool
gp_astrolabe_header_file_parser::
is_ready
(void)
const
{
  {
    return ready_;
  }
}

gp_astrolabe_header_file_parser::
gp_astrolabe_header_file_parser
(void)
{
  {
    file_name_header_ = "";
    file_name_schema_ = "";
    header_data_      = NULL;
    ready_            = false;
  }
}


gp_astrolabe_header_file_parser::
~gp_astrolabe_header_file_parser
(void)
{
  {
    list_of_errors_.clear();
    list_of_warnings_.clear();
  }
}

void
gp_astrolabe_header_file_parser::
parse
(void)
{
  set_precondition (is_ready());

  {
    // A general adp_DOMParser, used as the basic XML parsing tool.

    adp_DOMparser* dp;

    // A parsing utilities object.

    gp_utils       gpu;

    // Flag. Have we got an schema to validate the parsing process?

    bool           got_schema;

    //
    // Several TEMPORAL variables, used to store data read from
    // the input metadata file or to set the values of the
    // gp_metadata_osi object.
    //

    lineage_data*  lineage;

    int            idata;
    string         sattr;
    string         sdata;
    string         sdata2;

    // Variables involved in error messaging.

    int            istatus;
    vector<string> error_description;
    string         error_message;
    string         error_tag;
    bool           status;

    //
    // Instantiate the new adp_DOMparser. Be careful of instantiation
    // errors.
    //

    dp = new adp_DOMparser(file_name_header_.c_str());

    if (!dp->is_parser_valid())
    {
      queue_parsing_errors(dp);
      delete dp;
      dp = NULL;
      return;
    }

    // If we have an schema, inform the parser about it.

    got_schema = false;

    if (!file_name_schema_.empty())
    {
      //
      // The only possible reason to get an exception here is that
      // the schema file name is empty (precondition violation). This
      // should never happen, since we've already tested for a non
      // empty file name. Therefore, no try / cach sentence has been
      // included.
      //

      got_schema = true;
      dp->set_schema(file_name_schema_.c_str());
    }

    //
    // We are ready to parse ("load") the input file. Again, be careful
    // with parsing (loading) errors.
    //

    try
    {
      dp->parse();
    }
    catch (...)
    {

      // Problems parsing the input file. Report these and quit.

      queue_parsing_errors(dp);
      delete dp;
      dp = NULL;
      return;
    }

    //
    // At this point, the input file has been successfully loaded. It is
    // therefore possible to retrieve the information there stored.
    //
    //           ******** VERY IMPORTANT WARNING ********
    //
    // The code below is ASSUMING that a schema to validate the syntax of
    // the input file has been used. This means that there exist no
    // individual control code (try / catch clauses) to protect the
    // software against malformed input files.
    //
    // However, to enable quick software development when a metadata file
    // is modified, a GLOBAL try/catch clause surrounding the totality
    // of the parsing process is provided. This will avoid crashes, but
    // provide very litte information about the problem causing the
    // exception.
    //

    try
    {
      // Go to the starting label. Nothing to do with it.

      dp->goto_label("astrolabe-header_file");

      // Parse the lineage element

      //
      // Get a pointer to the lineage_data object included in the header_data_ object.
      // This simplifies the procedure of setting all its members.
      //

      dp->goto_label_relative("lineage");
      lineage = header_data_->get_lineage_ptr();

      status = gpu.parse_lineage(dp, lineage);

      if (!status) throw astrolabe_exception();

      dp->go_back(); // Back to astrolabe-header_file.

      //
      // Go to label data / device. There, we'll retrieve the
      // two mandatory attributes type and format to ascertain
      // what kind of data we have (type) and how it is stored /
      // handled (format).
      //

      dp->goto_label_relative("data/device");

      // Get the "type" attribute. Make it uppercase to simplify later handling.

      sattr = dp->_label_attribute_string("type");
      std::transform(sattr.begin(), sattr.end(),sattr.begin(), ::toupper);

      // Let's keep track of the type just received.

      header_data_->set_device_type(sattr);

      //
      // Get the format used by data. We'll make it uppercase to simplify later handling.
      //

      sattr = dp->_label_attribute_string("format");
      std::transform(sattr.begin(), sattr.end(),sattr.begin(), ::toupper);

      // Proceed according to the type just found.

      if (sattr == "TEXT_FILE")
      {
        // Set the device type to text file.

        header_data_->set_device_format(gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_TEXT_FILE);

        // Get the attached file name.

        sdata = dp->_label_value_string();
        sdata2 = gpu.string_whitespace_simplify(sdata);

        header_data_->set_device_file_name(sdata2);
      }
      else if (sattr == "BINARY_FILE")
      {
        // Set the device type to binary file.

        header_data_->set_device_format(gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_BINARY_FILE);

        // Get the attached file name.

        sdata = dp->_label_value_string();
        sdata2 = gpu.string_whitespace_simplify(sdata);

        header_data_->set_device_file_name(sdata2);
      }
      else if (sattr == "SOCKET")
      {
        // Set the device type to socket.

        header_data_->set_device_format(gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET);

        //
        // Get the attached server and port data. Note that the server may be empty, since it
        // is optional. This is not an error "per se"; depending on the kind of use that the
        // final user does of this socket, then a problem may arise. But not now.
        //

        sdata = dp->_label_value_string();
        sdata2 = gpu.string_whitespace_simplify(sdata);

        // Parse the server and port numbers.

        istatus = parse_socket_spec(sdata2, sdata, idata);

        if (istatus == 0)
        {
          // Correct parsing. Set the server and port values.

          if (sdata != "") header_data_->set_device_server(sdata);
          header_data_->set_device_port(idata);
        }
        else
        {
          // Invalid socket:port description.

          error_tag = "<astrolabe-header_file><data><device>";

          error_description.clear();
          error_description.push_back("<device> \"format\" is set to SOCKET.");
          error_description.push_back("Error while reading the socket:port specification.");

          switch (istatus)
          {
            case 1:
              
              error_description.push_back("No server's name or IP address found in spite that");
              error_description.push_back("a colon character has been included in the specification.");
              break;

            case 2:

              error_description.push_back("Nonexistent or invalid port number. It must be a valid integer.");
              break;

            default:
              break;
          }

          error_description.push_back("Please, correct this value in the input XML file.");

          error_message = gpu.build_message(2, file_name_header_, error_tag, error_description);

          list_of_errors_.push_back(error_message);
        }


      }

      dp->go_back(); // to astrolabe-header_file.

    }
    catch (...)
    {
      //
      // We've got an uncontrolled exception here.
      //
      // If the parser is being used WITHOUT an schema, this situation is not so
      // strange, since many assuptions made (as for instance, the presence of
      // some tags) may simply fail.
      //
      // On the contrary, if we're using an schema and reach this point, it
      // means that there is an error in the way the parser is programmed.
      //
      // At any rate, we'll add an error to the list of errors stating
      // that we've reached this point in the code and that the
      // parsing may not continue.
      //

      error_tag = "Unable to identify offending tag(s)";

      error_description.clear();

      if (!got_schema)
      {
        error_description.push_back("Unidentified exception catched. Since NO schema has been provided");
        error_description.push_back("the parser may have failed because an erroneous input file.");
        error_description.push_back("Are you a developer modifying this software?");
      }
      else
      {
        error_description.push_back("Sorry: unidentified error found. Revise your input file carefully!");
      }

      error_message = gpu.build_message(2, file_name_header_, error_tag, error_description);

      list_of_errors_.push_back(error_message);
    }


    //
    // At this point, all possible values have been fetched, either
    // successfully or not.
    //
    // When errors have been detected, the size of list_of_errors_ will
    // be greater than zero. If so, we cannot continue and must return.
    //

    if (list_of_errors_.size() > 0)
    {
      delete dp;
      dp = NULL;

      return;
    }

    //
    // No parsing errors! Now we must check the semantic validity of the
    // options just read.
    //
    // validate_semantics() will take care of such task. If problems are
    // detected, these wil be added to either the list of errors or
    // warnings.
    //

    validate_semantics();


    //
    // That's all. Destroy the parser.
    //

    delete dp;
    dp = NULL;

    return;
  }
}

int
gp_astrolabe_header_file_parser::
parse_socket_spec
(string& socket_spec,
 string& server,
 int&    port)
{
  {
    int colon_position;
    int start, end, length;
    string the_server;
    string the_string_port;
    int    the_port;

    //
    // Look for the colon used to separate the server and port parts.
    // If it is not found, nothing happens: presumably, we only
    // have a port spec.
    //

    colon_position = (int)socket_spec.find(":");
    if (colon_position == (int)(string::npos))
    {
      //
      // Since no colon has been found, we'll assume that the specification
      // we are processing includes a port number only, since it's the
      // only possible legal situation in this case.
      //
      // As a consequence, the server must be set to "nothing"
      //

      the_server = "";

      //
      // Assign the fake position of the colon character so the search for
      // the port  number works in all cases.
      //

      colon_position = -1;
    }
    else
    {

      //
      // The server part goes from the start of the string to the position
      // just before the colon found.
      //

      the_server = socket_spec.substr(0, colon_position);

      //
      // Remove any whitespace that might be at the beginning
      // or the end of the server name.
      //

      start = (int)the_server.find_first_not_of(" \t\r\n");
      if (start == (int)(string::npos)) start = 0;

      end   = (int)the_server.find_first_of(" \t\r\n", start + 1);
      if (end == (int)(string::npos)) end = (int)the_server.length();

      length = end - start;

      //
      // Hey! If once all the whitespace has been removed we have no
      // characters left, we have a problem!
      //

      if (length <= 0) return 1; // Invalid syntax, no server name.

      // Well, now we have a clean and polished server name:

      the_server = the_server.substr(start, length);
    }

    //
    // Let's go for the port number. It must be a integer.
    //

    //
    // First copy in a local string the part of the input one that
    // goes from just after the colon to its very end.
    //

    the_string_port = socket_spec.substr(colon_position + 1);

    //
    // Try to convert it to integer form using a istringstream.
    // If this fails, we've got a problem.
    //

    istringstream ss(the_string_port);
    if (!(ss >> the_port)) return 2; // Syntax error: no port or not an integer.

    // Well, we DO have a server and a port. Copy results to output parameters...

    server = the_server;
    port   = the_port;

    // That's all.

    return 0;
  }
}

void
gp_astrolabe_header_file_parser::
queue_parsing_errors
(adp_DOMparser* dp)
{
  {
    string              error_description;
    int                 i;
    adp_parsing_errors* pe;
    astrolabe_parse_error*    pex;

    pe = dp->_parsing_errors();

    for (i=1; i<=pe->_size(); i++)
    {
      pex = pe->get_error(i);
      error_description = (*pex)._description();

      list_of_errors_.push_back(error_description);

      delete pex;
    }
    delete pe;
  }
}

void
gp_astrolabe_header_file_parser::
set_parser_parameters
(string& file_name_header,
 string& file_name_schema,
 gp_astrolabe_header_file_data* header_data)
{
  set_precondition (!file_name_header.empty());
  set_precondition (!file_name_schema.empty());
  set_precondition (header_data != NULL);

  {
    file_name_header_ = file_name_header;
    file_name_schema_ = file_name_schema;
    header_data_      = header_data;
    ready_            = true;
  }

  set_postcondition (is_ready());
}

void
gp_astrolabe_header_file_parser::
set_parser_parameters_developer
(string& file_name_header,
 gp_astrolabe_header_file_data* header_data)
{
  set_precondition (!file_name_header.empty());
  set_precondition (header_data != NULL);

  {
    file_name_header_ = file_name_header;
    header_data_      = header_data;
    ready_            = true;
  }

  set_postcondition (is_ready());
}

void
gp_astrolabe_header_file_parser::
validate_semantics
(void)
{
  {

    //
    // Valid range for socket ports. Assigned just after the last declaration.
    // Change the assignments is the valid range of socket ports is
    // modified.
    //

    int                            min_sock_port;
    int                            max_sock_port;

    // Temporal variables.

    int                            idata;

    // Error-related variables.

    vector<string>                 error_description;
    string                         error_message;
    string                         error_tag;

    // GEMMA parsing utilities object.

    gp_utils                       gpu;

    //
    // Assign the range of valid socket ports. Change these limits each
    // time the range is modified.
    //

    min_sock_port = 1;
    max_sock_port = 65535;

    //
    //  -----------------------------------------------------------------
    //  ------ FIRST, CHECK THOSE CONDITIONS THAT ARE FATAL ERRORS ------
    //  -----------------------------------------------------------------
    //

    //
    // When sockets, the port number must be within the allowed range.
    //

    if (header_data_->get_device_format() == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET)
    {
      idata = header_data_->get_device_port();

      if ((idata < min_sock_port) | (idata > max_sock_port))
      {
        // Invalid socket port number.

        error_tag = "<astrolabe-header_file><data><device>";

        error_description.clear();
        error_description.push_back("Socket port out of valid range.");
        error_description.push_back("Range is 1..65535. Values from 1 to 1024 are usually reserved.");

        error_message = gpu.build_message(2, file_name_header_, error_tag, error_description);

        list_of_errors_.push_back(error_message);
      }
    }


    //
    //  -------------------------------------------------------------------
    //  ------ SECOND, CHECK THOSE CONDITIONS THAT ARE JUST WARNINGS ------
    //  -------------------------------------------------------------------
    //

    //
    // When sockets, the port number should be > 1024
    //

    if (header_data_->get_device_format() == gp_astrolabe_header_file_data::GP_ASTROLABE_HEADER_FORMAT_IS_SOCKET)
    {
      idata = header_data_->get_device_port();

      if (idata <= 1024)
      {
        // "Dangerous" port number. May clash with reserved ones.

        error_tag = "<astrolabe-header_file><data><device>";

        error_description.clear();
        error_description.push_back("Socket port number should be greater than 1024 to avoid a clash");
        error_description.push_back("with standard services that usually use this range of values.");

        error_message = gpu.build_message(1, file_name_header_, error_tag, error_description);

        list_of_warnings_.push_back(error_message);
      }
    }

  }
}

void
gp_astrolabe_header_file_parser::
warnings_add
(string& the_warning)
{
  {
    list_of_warnings_.push_back(the_warning);
  }
}

int
gp_astrolabe_header_file_parser::
warnings_dim
(void)
const
{
  {
    return (int)list_of_warnings_.size();
  }
}

string
gp_astrolabe_header_file_parser::
warnings_get
(int at_position)
{
  set_precondition (warnings_dim() > 0);
  set_precondition (at_position >= 0);
  set_precondition (at_position < warnings_dim());

  string result;

  {
    result = list_of_warnings_[at_position];
  }

  return result;
}

