#ifndef SURF_QUERY_PARSER_HPP
#define SURF_QUERY_PARSER_HPP

namespace surf{

struct query_parser {
    using query_t = std::pair<uint64_t,std::vector<uint64_t>>;
    query_parser() = delete;
    static std::vector<query_t> parse_queries(const std::string& collection_dir,
                                            const std::string& query_file) 
    {
        std::vector<query_t> queries;

        /* load the mapping */
        std::unordered_map<std::string,uint64_t> id_mapping;
        {
            auto dict_file = collection_dir + "/" + surf::DICT_FILENAME;
            std::ifstream dfs(dict_file);
            if(!dfs.is_open()) {
                std::cerr << "cannot load dictionary file.";
                exit(EXIT_FAILURE);
            }
            std::string term_mapping;
            while( std::getline(dfs,term_mapping) ) {
                auto sep_pos = term_mapping.find(' ');
                auto term = term_mapping.substr(0,sep_pos-1);
                auto idstr = term_mapping.substr(sep_pos+1);
                uint64_t id = std::stoull(idstr);
                id_mapping[term] = id;
            }
        }

        /* parse queries */
        std::ifstream qfs(query_file); 
        if(!qfs.is_open()) {
            std::cerr << "cannot load query file.";
            exit(EXIT_FAILURE);
        }
        std::string query_str;
        while( std::getline(qfs,query_str) ) {
            auto id_sep_pos = query_str.find(';');
            auto qryid_str = query_str.substr(0,id_sep_pos);
            auto qry_id = std::stoull(qryid_str);
            std::istringstream qry_content_stream(query_str.substr(id_sep_pos+1));
            std::vector<uint64_t> qry_content;
            for(std::string qry_token; std::getline(qry_content_stream,qry_token,' ');) {
                auto id_itr = id_mapping.find(qry_token);
                if(id_itr != id_mapping.end()) {
                    qry_content.push_back(id_itr->second);
                } else {
                    std::cerr << "ERROR: could not find '" << qry_token << "' in the dictionary." << std::endl;
                }
            }
            if(!qry_content.empty()) queries.emplace_back(qry_id,qry_content);
        }

        return queries;
    }
};

}// end namespace

#endif
