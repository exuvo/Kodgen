
inline FieldParsingResult* FieldParser2::getParsingResult() noexcept
{
	return reinterpret_cast<FieldParsingResult*>(getContext().parsingResult);
}