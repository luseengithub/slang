#include "slang.h"

namespace slang {

const char* GetTokenKindText(TokenKind kind) {
    switch (kind) {
        // punctuation
        case TokenKind::ApostropheOpenBrace: return "'{";
        case TokenKind::OpenBrace: return "{";
        case TokenKind::CloseBrace: return "}";
        case TokenKind::OpenBracket: return "[";
        case TokenKind::CloseBracket: return "]";
        case TokenKind::OpenParenthesis: return "(";
        case TokenKind::OpenParenthesisStar: return "(*";
        case TokenKind::CloseParenthesis: return ")";
        case TokenKind::StarCloseParenthesis: return "*)";
        case TokenKind::Semicolon: return ";";
        case TokenKind::Colon: return ":";
        case TokenKind::ColonEquals: return ":=";
        case TokenKind::ColonSlash: return ":/";
        case TokenKind::DoubleColon: return "::";
        case TokenKind::StarDoubleColonStar: return "*::*";
        case TokenKind::Comma: return ",";
        case TokenKind::DotStar: return ".*";
        case TokenKind::Dot: return ".";
        case TokenKind::Slash: return "/";
        case TokenKind::Star: return "*";
        case TokenKind::DoubleStar: return "**";
        case TokenKind::StarArrow: return "*>";
        case TokenKind::Plus: return "+";
        case TokenKind::DoublePlus: return "++";
        case TokenKind::PlusColon: return "+:";
        case TokenKind::Minus: return "-";
        case TokenKind::DoubleMinus: return "--";
        case TokenKind::MinusColon: return "-:";
        case TokenKind::MinusArrow: return "->";
        case TokenKind::MinusDoubleArrow: return "->>";
        case TokenKind::Tilde: return "~";
        case TokenKind::TildeAnd: return "~&";
        case TokenKind::TildeOr: return "~|";
        case TokenKind::TildeXor: return "~^";
        case TokenKind::Dollar: return "$";
        case TokenKind::Question: return "?";
        case TokenKind::Hash: return "#";
        case TokenKind::DoubleHash: return "##";
        case TokenKind::HashMinusHash: return "#-#";
        case TokenKind::HashEqualsHash: return "#=#";
        case TokenKind::Xor: return "^";
        case TokenKind::XorTilde: return "^~";
        case TokenKind::Equals: return "=";
        case TokenKind::DoubleEquals: return "==";
        case TokenKind::DoubleEqualsQuestion: return "==?";
        case TokenKind::TripleEquals: return "===";
        case TokenKind::EqualsArrow: return "=>";
        case TokenKind::PlusEqual: return "+=";
        case TokenKind::MinusEqual: return "-=";
        case TokenKind::SlashEqual: return "/=";
        case TokenKind::StarEqual: return "*=";
        case TokenKind::AndEqual: return "&=";
        case TokenKind::OrEqual: return "|=";
        case TokenKind::PercentEqual: return "%=";
        case TokenKind::XorEqual: return "^=";
        case TokenKind::LeftShiftEqual: return "<<=";
        case TokenKind::TripleLeftShiftEqual: return "<<<=";
        case TokenKind::RightShiftEqual: return ">>=";
        case TokenKind::TripleRightShiftEqual: return ">>>=";
        case TokenKind::LeftShift: return "<<";
        case TokenKind::RightShift: return ">>";
        case TokenKind::TripleLeftShift: return "<<<";
        case TokenKind::TripleRightShift: return ">>>";
        case TokenKind::Exclamation: return "!";
        case TokenKind::ExclamationEquals: return "!=";
        case TokenKind::ExclamationEqualsQuestion: return "!=?";
        case TokenKind::ExclamationDoubleEquals: return "!==";
        case TokenKind::Percent: return "%";
        case TokenKind::LessThan: return "<";
        case TokenKind::LessThanEquals: return "<=";
        case TokenKind::LessThanMinusArrow: return "<->";
        case TokenKind::GreaterThan: return ">";
        case TokenKind::GreaterThanEquals: return ">=";
        case TokenKind::Or: return "|";
        case TokenKind::DoubleOr: return "||";
        case TokenKind::OrMinusArrow: return "|->";
        case TokenKind::OrEqualsArrow: return "|=>";
        case TokenKind::At: return "@";
        case TokenKind::DoubleAt: return "@@";
        case TokenKind::And: return "&";
        case TokenKind::DoubleAnd: return "&&";
        case TokenKind::TripleAnd: return "&&&";

        // keywords
        case TokenKind::AcceptOnKeyword: return "accept_on";
        case TokenKind::AliasKeyword: return "alias";
        case TokenKind::AlwaysKeyword: return "always";
        case TokenKind::AlwaysCombKeyword: return "always_comb";
        case TokenKind::AlwaysFFKeyword: return "always_ff";
        case TokenKind::AlwaysLatchKeyword: return "always_latch";
        case TokenKind::AndKeyword: return "and";
        case TokenKind::AssertKeyword: return "assert";
        case TokenKind::AssignKeyword: return "assign";
        case TokenKind::AssumeKeyword: return "assume";
        case TokenKind::AutomaticKeyword: return "automatic";
        case TokenKind::BeforeKeyword: return "before";
        case TokenKind::BeginKeyword: return "begin";
        case TokenKind::BindKeyword: return "bind";
        case TokenKind::BinsKeyword: return "bins";
        case TokenKind::BinsOfKeyword: return "binsof";
        case TokenKind::BitKeyword: return "bit";
        case TokenKind::BreakKeyword: return "break";
        case TokenKind::BufKeyword: return "buf";
        case TokenKind::BufIf0Keyword: return "bufif0";
        case TokenKind::BufIf1Keyword: return "bufif1";
        case TokenKind::ByteKeyword: return "byte";
        case TokenKind::CaseKeyword: return "case";
        case TokenKind::CaseXKeyword: return "casex";
        case TokenKind::CaseZKeyword: return "casez";
        case TokenKind::CellKeyword: return "cell";
        case TokenKind::CHandleKeyword: return "chandle";
        case TokenKind::CheckerKeyword: return "checker";
        case TokenKind::ClassKeyword: return "class";
        case TokenKind::ClockingKeyword: return "clocking";
        case TokenKind::CmosKeyword: return "cmos";
        case TokenKind::ConfigKeyword: return "config";
        case TokenKind::ConstKeyword: return "const";
        case TokenKind::ConstraintKeyword: return "constraint";
        case TokenKind::ContextKeyword: return "context";
        case TokenKind::ContinueKeyword: return "continue";
        case TokenKind::CoverKeyword: return "cover";
        case TokenKind::CoverGroupKeyword: return "covergroup";
        case TokenKind::CoverPointKeyword: return "coverpoint";
        case TokenKind::CrossKeyword: return "cross";
        case TokenKind::DeassignKeyword: return "deassign";
        case TokenKind::DefaultKeyword: return "default";
        case TokenKind::DefParamKeyword: return "defparam";
        case TokenKind::DesignKeyword: return "design";
        case TokenKind::DisableKeyword: return "disable";
        case TokenKind::DistKeyword: return "dist";
        case TokenKind::DoKeyword: return "do";
        case TokenKind::EdgeKeyword: return "edge";
        case TokenKind::ElseKeyword: return "else";
        case TokenKind::EndKeyword: return "end";
        case TokenKind::EndCaseKeyword: return "endcase";
        case TokenKind::EndCheckerKeyword: return "endchecker";
        case TokenKind::EndClassKeyword: return "endclass";
        case TokenKind::EndClockingKeyword: return "endclocking";
        case TokenKind::EndConfigKeyword: return "endconfig";
        case TokenKind::EndFunctionKeyword: return "endfunction";
        case TokenKind::EndGenerateKeyword: return "endgenerate";
        case TokenKind::EndGroupKeyword: return "endgroup";
        case TokenKind::EndInterfaceKeyword: return "endinterface";
        case TokenKind::EndModuleKeyword: return "endmodule";
        case TokenKind::EndPackageKeyword: return "endpackage";
        case TokenKind::EndPrimitiveKeyword: return "endprimitive";
        case TokenKind::EndProgramKeyword: return "endprogram";
        case TokenKind::EndPropertyKeyword: return "endproperty";
        case TokenKind::EndSpecifyKeyword: return "endspecify";
        case TokenKind::EndSequenceKeyword: return "endsequence";
        case TokenKind::EndTableKeyword: return "endtable";
        case TokenKind::EndTaskKeyword: return "endtask";
        case TokenKind::EnumKeyword: return "enum";
        case TokenKind::EventKeyword: return "event";
        case TokenKind::EventuallyKeyword: return "eventually";
        case TokenKind::ExpectKeyword: return "expect";
        case TokenKind::ExportKeyword: return "export";
        case TokenKind::ExtendsKeyword: return "extends";
        case TokenKind::ExternKeyword: return "extern";
        case TokenKind::FinalKeyword: return "final";
        case TokenKind::FirstMatchKeyword: return "first_match";
        case TokenKind::ForKeyword: return "for";
        case TokenKind::ForceKeyword: return "force";
        case TokenKind::ForeachKeyword: return "foreach";
        case TokenKind::ForeverKeyword: return "forever";
        case TokenKind::ForkKeyword: return "fork";
        case TokenKind::ForkJoinKeyword: return "forkjoin";
        case TokenKind::FunctionKeyword: return "function";
        case TokenKind::GenerateKeyword: return "generate";
        case TokenKind::GenVarKeyword: return "genvar";
        case TokenKind::GlobalKeyword: return "global";
        case TokenKind::HighZ0Keyword: return "highz0";
        case TokenKind::HighZ1Keyword: return "highz1";
        case TokenKind::IfKeyword: return "if";
        case TokenKind::IffKeyword: return "iff";
        case TokenKind::IfNoneKeyword: return "ifnone";
        case TokenKind::IgnoreBinsKeyword: return "ignore_bins";
        case TokenKind::IllegalBinsKeyword: return "illegal_bins";
        case TokenKind::ImplementsKeyword: return "implements";
        case TokenKind::ImpliesKeyword: return "implies";
        case TokenKind::ImportKeyword: return "import";
        case TokenKind::IncDirKeyword: return "incdir";
        case TokenKind::IncludeKeyword: return "include";
        case TokenKind::InitialKeyword: return "initial";
        case TokenKind::InOutKeyword: return "inout";
        case TokenKind::InputKeyword: return "input";
        case TokenKind::InsideKeyword: return "inside";
        case TokenKind::InstanceKeyword: return "instance";
        case TokenKind::IntKeyword: return "int";
        case TokenKind::IntegerKeyword: return "integer";
        case TokenKind::InterconnectKeyword: return "interconnect";
        case TokenKind::InterfaceKeyword: return "interface";
        case TokenKind::IntersectKeyword: return "intersect";
        case TokenKind::JoinKeyword: return "join";
        case TokenKind::JoinAnyKeyword: return "join_any";
        case TokenKind::JoinNoneKeyword: return "join_none";
        case TokenKind::LargeKeyword: return "large";
        case TokenKind::LetKeyword: return "let";
        case TokenKind::LibListKeyword: return "liblist";
        case TokenKind::LibraryKeyword: return "library";
        case TokenKind::LocalKeyword: return "local";
        case TokenKind::LocalParamKeyword: return "localparam";
        case TokenKind::LogicKeyword: return "logic";
        case TokenKind::LongIntKeyword: return "longint";
        case TokenKind::MacromoduleKeyword: return "macromodule";
        case TokenKind::MatchesKeyword: return "matches";
        case TokenKind::MediumKeyword: return "medium";
        case TokenKind::ModPortKeyword: return "modport";
        case TokenKind::ModuleKeyword: return "module";
        case TokenKind::NandKeyword: return "nand";
        case TokenKind::NegEdgeKeyword: return "negedge";
        case TokenKind::NetTypeKeyword: return "nettype";
        case TokenKind::NewKeyword: return "new";
        case TokenKind::NextTimeKeyword: return "nexttime";
        case TokenKind::NmosKeyword: return "nmos";
        case TokenKind::NorKeyword: return "nor";
        case TokenKind::NoShowCancelledKeyword: return "noshowacncelled";
        case TokenKind::NotKeyword: return "not";
        case TokenKind::NotIf0Keyword: return "notif0";
        case TokenKind::NotIf1Keyword: return "notif1";
        case TokenKind::NullKeyword: return "null";
        case TokenKind::OrKeyword: return "or";
        case TokenKind::OutputKeyword: return "output";
        case TokenKind::PackageKeyword: return "package";
        case TokenKind::PackedKeyword: return "packed";
        case TokenKind::ParameterKeyword: return "parameter";
        case TokenKind::PmosKeyword: return "pmos";
        case TokenKind::PosEdgeKeyword: return "posedge";
        case TokenKind::PrimitiveKeyword: return "primitive";
        case TokenKind::PriorityKeyword: return "priority";
        case TokenKind::ProgramKeyword: return "program";
        case TokenKind::PropertyKeyword: return "property";
        case TokenKind::ProtectedKeyword: return "protected";
        case TokenKind::Pull0Keyword: return "pull0";
        case TokenKind::Pull1Keyword: return "pull1";
        case TokenKind::PullDownKeyword: return "pulldown";
        case TokenKind::PullUpKeyword: return "pullup";
        case TokenKind::PulseStyleOnDetectKeyword: return "pulsestyle_ondetect";
        case TokenKind::PulseStyleOnEventKeyword: return "pulsestyle_onevent";
        case TokenKind::PureKeyword: return "pure";
        case TokenKind::RandKeyword: return "rand";
        case TokenKind::RandCKeyword: return "randc";
        case TokenKind::RandCaseKeyword: return "randcase";
        case TokenKind::RandSequenceKeyword: return "randsequence";
        case TokenKind::RcmosKeyword: return "rcmos";
        case TokenKind::RealKeyword: return "real";
        case TokenKind::RealTimeKeyword: return "realtime";
        case TokenKind::RefKeyword: return "ref";
        case TokenKind::RegKeyword: return "reg";
        case TokenKind::RejectOnKeyword: return "reject";
        case TokenKind::ReleaseKeyword: return "release";
        case TokenKind::RepeatKeyword: return "repeat";
        case TokenKind::RestrictKeyword: return "restrict";
        case TokenKind::ReturnKeyword: return "return";
        case TokenKind::RnmosKeyword: return "rnmos";
        case TokenKind::RpmosKeyword: return "rpmos";
        case TokenKind::RtranKeyword: return "rtran";
        case TokenKind::RtranIf0Keyword: return "rtranif0";
        case TokenKind::RtranIf1Keyword: return "rtranif1";
        case TokenKind::SAlwaysKeyword: return "s_always";
        case TokenKind::SEventuallyKeyword: return "s_eventually";
        case TokenKind::SNextTimeKeyword: return "s_nexttime";
        case TokenKind::SUntilKeyword: return "s_until";
        case TokenKind::SUntilWithKeyword: return "s_until_with";
        case TokenKind::ScalaredKeyword: return "scalared";
        case TokenKind::SequenceKeyword: return "sequence";
        case TokenKind::ShortIntKeyword: return "shortint";
        case TokenKind::ShortRealKeyword: return "shortreal";
        case TokenKind::ShowCancelledKeyword: return "showcancelled";
        case TokenKind::SignedKeyword: return "signed";
        case TokenKind::SmallKeyword: return "small";
        case TokenKind::SoftKeyword: return "soft";
        case TokenKind::SolveKeyword: return "solve";
        case TokenKind::SpecifyKeyword: return "specify";
        case TokenKind::SpecParamKeyword: return "specparam";
        case TokenKind::StaticKeyword: return "static";
        case TokenKind::StringKeyword: return "string";
        case TokenKind::StrongKeyword: return "strong";
        case TokenKind::Strong0Keyword: return "strong0";
        case TokenKind::Strong1Keyword: return "strong1";
        case TokenKind::StructKeyword: return "struct";
        case TokenKind::SuperKeyword: return "super";
        case TokenKind::Supply0Keyword: return "supply0";
        case TokenKind::Supply1Keyword: return "supply1";
        case TokenKind::SyncAcceptOnKeyword: return "sync_accept_on";
        case TokenKind::SyncRejectOnKeyword: return "sync_reject_on";
        case TokenKind::TableKeyword: return "table";
        case TokenKind::TaggedKeyword: return "tagged";
        case TokenKind::TaskKeyword: return "task";
        case TokenKind::ThisKeyword: return "this";
        case TokenKind::ThroughoutKeyword: return "throughout";
        case TokenKind::TimeKeyword: return "time";
        case TokenKind::TimePrecisionKeyword: return "timeprecision";
        case TokenKind::TimeUnitKeyword: return "timeunit";
        case TokenKind::TranKeyword: return "tran";
        case TokenKind::TranIf0Keyword: return "tranif0";
        case TokenKind::TranIf1Keyword: return "tranif1";
        case TokenKind::TriKeyword: return "tri";
        case TokenKind::Tri0Keyword: return "tri0";
        case TokenKind::Tri1Keyword: return "tri1";
        case TokenKind::TriAndKeyword: return "triand";
        case TokenKind::TriOrKeyword: return "trior";
        case TokenKind::TriRegKeyword: return "trireg";
        case TokenKind::TypeKeyword: return "type";
        case TokenKind::TypedefKeyword: return "typedef";
        case TokenKind::UnionKeyword: return "union";
        case TokenKind::UniqueKeyword: return "unique";
        case TokenKind::Unique0Keyword: return "unique0";
        case TokenKind::UnsignedKeyword: return "unsigned";
        case TokenKind::UntilKeyword: return "until";
        case TokenKind::UntilWithKeyword: return "until_with";
        case TokenKind::UntypedKeyword: return "untyped";
        case TokenKind::UseKeyword: return "use";
        case TokenKind::UWireKeyword: return "uwire";
        case TokenKind::VarKeyword: return "var";
        case TokenKind::VectoredKeyword: return "vectored";
        case TokenKind::VirtualKeyword: return "virtual";
        case TokenKind::VoidKeyword: return "void";
        case TokenKind::WaitKeyword: return "wait";
        case TokenKind::WaitOrderKeyword: return "wait_order";
        case TokenKind::WAndKeyword: return "wand";
        case TokenKind::WeakKeyword: return "weak";
        case TokenKind::Weak0Keyword: return "weak0";
        case TokenKind::Weak1Keyword: return "weak1";
        case TokenKind::WhileKeyword: return "while";
        case TokenKind::WildcardKeyword: return "wildcard";
        case TokenKind::WireKeyword: return "wire";
        case TokenKind::WithKeyword: return "with";
        case TokenKind::WithinKeyword: return "within";
        case TokenKind::WOrKeyword: return "wor";
        case TokenKind::XnorKeyword: return "xnor";
        case TokenKind::XorKeyword: return "xor";

        // predefined system keywords
        case TokenKind::UnitSystemName: return "$unit";
        case TokenKind::RootSystemName: return "$root";

        // directives
        case TokenKind::MacroQuote: return "`\"";
        case TokenKind::MacroEscapedQuote: return "`\\`\"";
        case TokenKind::MacroPaste: return "``";

        default: return nullptr;
    }
}

}