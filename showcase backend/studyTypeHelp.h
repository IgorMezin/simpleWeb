
enum class studyType : int {
    st_undefined = 0,
    st_target = 1,
    st_preferent = 2,
    st_budget = 3,
    st_contract = 4
};

#define CASE_STUDY_TYPE_RETURN(name) case studyType::st_##name: return studyType::st_##name

studyType convertIntToStudyType(const int val)
{
    switch (static_cast<studyType>(val)) {
        CASE_STUDY_TYPE_RETURN(undefined);
        CASE_STUDY_TYPE_RETURN(target);
        CASE_STUDY_TYPE_RETURN(preferent);
        CASE_STUDY_TYPE_RETURN(budget);
        CASE_STUDY_TYPE_RETURN(contract);
    default:
        std::cerr << "undefined StudyType" << std::endl;
        return studyType::st_undefined;
    }
}
std::string convertStudyTypeToStr(const studyType sttype) {
    switch (sttype) {
    case(studyType::st_undefined): return "undefined";
    case(studyType::st_target): return "target";
    case(studyType::st_preferent): return "preferent";
    case(studyType::st_budget): return "budget";
    case(studyType::st_contract): return "contract";
    default:{
        std::cerr << "undefined StudyType" << std::endl;
        return "undefined";
    }
    }
}

std::string convertStudyTypeToStrRu(const studyType sttype) {
    switch (sttype) {
    case(studyType::st_undefined): return "undefined";
    case(studyType::st_target): return "целевое";
    case(studyType::st_preferent): return "льготное";
    case(studyType::st_budget): return "бюджетное";
    case(studyType::st_contract): return "контрактное";
    default: {
        std::cerr << "undefined StudyType" << std::endl;
        return "undefined";
    }
    }
}
static std::vector<studyType> priorityStudyTypes = { studyType::st_target, studyType::st_preferent, studyType::st_budget, studyType::st_contract };
