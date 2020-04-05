/* main.c  -  Foundation stream test  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <test/test.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#endif

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

#if FOUNDATION_COMPILER_CLANG
// Yes, we want to compare floats
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif

static const char longline[] =
    "Lorem ipsum dolor sit amet, eam mundi concludaturque id, cu sea modus saepe postea. Purto natum vel ex, est eius "
    "possim ex, vix at sumo reformidans. Eos oratio forensibus persequeris an, diam gubergren ne mea, tacimates "
    "postulant expetenda ut pri. Vel everti facilis nominati ea, eos eu quis molestiae assueverit. His paulo utroque "
    "officiis ut, eum eu mutat congue hendrerit. Luptatum consulatu in cum, ea posse pertinax vim."
    " Usu dicta audiam voluptatum in. Vis habeo molestiae rationibus ea. Paulo bonorum facilis cu quo. Sit in appetere "
    "concludaturque, ius cu zril invidunt patrioque, clita ceteros recteque sed in. Vim in graece prompta molestiae, "
    "vel enim magna ei."
    " Ne qui dicant comprehensam, ipsum scriptorem mei no. Odio appareat pericula no sit. Vix ad latine persius "
    "intellegam, eam movet altera ei. Minimum copiosae has no, ne utamur aliquando eam."
    " Verterem lucilius adversarium vix ne, brute laudem ex vel. Aperiri quaeque scribentur ei pri, ne mei copiosae "
    "complectitur. Ferri graece sed an. An usu etiam audire erroribus, vis commodo pertinax id."
    " Dicunt inimicus ei sit. Consequat prodesset te pri, dictas vidisse fierent est et. Cu qui omittam evertitur. Vim "
    "ad vidit erant expetenda. Ex duo nibh molestiae tincidunt."
    " Vel ad utroque inimicus definitionem, et posse consul aliquip per. Ne quaeque epicuri signiferumque est. Pri "
    "saepe doming partiendo ei. Ea nec vidit suscipit erroribus, odio nihil corrumpit qui ex, ne consulatu assentior "
    "pri. Eu qui dolore ubique, eos option albucius et."
    " Sea recteque cotidieque ea, mei tota paulo intellegebat ei, cu eam tation partem abhorreant. Mel luptatum "
    "mnesarchum liberavisse no, quis regione detraxit nam te. Nec ad everti nonumes, euismod tincidunt duo eu. Ea "
    "illud petentium pro, ex pro soluta necessitatibus."
    " Eum cu nonumes delicata, reque omnium fastidii in mel, eu eum dolore euismod. Noster insolens accusata in pri, "
    "eam dicam exerci instructior in. No posse maluisset abhorreant has, qui denique assentior ea, sea labore legendos "
    "vituperata te. Facer scripserit vis in."
    " Verterem mnesarchum scriptorem eam ex, pro semper theophrastus id. Sit ex inani iuvaret interpretaris, rebum "
    "labores consulatu te qui. Eu errem perpetua nam. Veniam elaboraret vel eu. Cum tation semper ex."
    " Illud eirmod ius at. Soleat animal eripuit mei ea, duo nominavi adipiscing eu. Utroque voluptatum no nam. Usu "
    "omnesque iracundia ad."
    " Magna detraxit ex nec, et nec impedit civibus scribentur. Sea et nibh appareat, postea sadipscing signiferumque "
    "an vim, alii vidisse qualisque eos ei. Nisl mnesarchum ad cum. Sint esse causae ius ex, scripta omittam probatus "
    "vix cu, legere persequeris ea mea."
    " Ad vel iriure propriae, et cum eripuit partiendo assueverit. Vix an velit dolorum prodesset, usu at utinam "
    "partem. Ius vide veniam tempor cu. Ut electram hendrerit est, est altera verterem in. Ius audire definiebas ea, "
    "eos ea semper moderatius intellegebat, his ei ludus libris fabellas. Vix cu malorum voluptaria, ne vix nostrum "
    "pericula persequeris."
    " Cum habemus fuisset delectus at. Vix dicta inani omnes ea, graecis offendit evertitur cu mel, no offendit "
    "oporteat mea. An alterum facilisis qui. Sed dicta mundi ubique cu, cu sonet everti regione vix. Qui intellegat "
    "adolescens at, ipsum tation interpretaris usu an, natum suscipit sadipscing est et. Mea esse scripta antiopam in, "
    "adhuc appetere moderatius mei eu."
    " Augue populo at mei, hinc graece qui id, cu cum summo epicurei intellegam. At vim habeo ullum labitur, in est "
    "aliquam fierent legendos, ne eam homero dolorem. Quo eu partiendo patrioque eloquentiam. Sonet prodesset "
    "posidonium mei ea, an mei vidit definiebas vituperata. Ne doming ullamcorper vim, et purto sale dicunt sit. Id "
    "legere perpetua est, mel cu erant petentium deterruisset. Ei eos errem vitae feugiat, te brute luptatum "
    "scripserit sit, per lorem persius ea."
    " In quo doctus invenire. Eos ancillae definiebas interesset at. Primis mucius mel ad, ea audiam nominavi has, "
    "veniam fuisset ei mei. Ut clita affert aliquando pro, no quem vero sea. Quas ridens dissentiunt ius ne, eos "
    "rationibus incorrupte ex, sea ei aperiri constituto.";
static const unsigned int longlength = sizeof(longline) - 1;

static application_t
test_stream_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation stream tests"));
	app.short_name = string_const(STRING_CONST("test_stream"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_stream_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_stream_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_stream_initialize(void) {
	return 0;
}

static void
test_stream_finalize(void) {
}

DECLARE_TEST(stream, std) {
	tick_t now;
	stream_t* in;
	stream_t* out;
	stream_t* err;
	stream_t* in_clone;
	stream_t* out_clone;
	stream_t* err_clone;

	in = stream_open(STRING_CONST("stdin://"), STREAM_OUT | STREAM_IN | STREAM_BINARY);
	out = stream_open(STRING_CONST("stdout://"), STREAM_OUT | STREAM_IN);
	err = stream_open(STRING_CONST("stderr://"), STREAM_OUT | STREAM_IN | STREAM_BINARY);

	EXPECT_NE_MSG(in, 0, "Failed to open stdin://");
	EXPECT_NE_MSG(out, 0, "Failed to open stdout://");
	EXPECT_NE_MSG(err, 0, "Failed to open stderr://");

	EXPECT_NE_MSG(in, 0, "Failed to open stdin://");
	EXPECT_NE_MSG(out, 0, "Failed to open stdout://");
	EXPECT_NE_MSG(err, 0, "Failed to open stderr://");

	EXPECT_EQ_MSG(stream_tell(in), 0, "stdin pos is not 0");
	EXPECT_EQ_MSG(stream_tell(out), 0, "stdout pos is not 0");
	EXPECT_EQ_MSG(stream_tell(err), 0, "stderr pos is not 0");

	stream_seek(in, 1, STREAM_SEEK_BEGIN);
	stream_seek(out, 1, STREAM_SEEK_CURRENT);
	stream_seek(err, 1, STREAM_SEEK_END);

	EXPECT_EQ_MSG(stream_tell(in), 0, "stdin pos is not 0");
	EXPECT_EQ_MSG(stream_tell(out), 0, "stdout pos is not 0");
	EXPECT_EQ_MSG(stream_tell(err), 0, "stderr pos is not 0");

	EXPECT_EQ_MSG(stream_eos(in), false, "stdin at eos");
	EXPECT_EQ_MSG(stream_eos(out), false, "stdout at eos");
	EXPECT_EQ_MSG(stream_eos(err), false, "stderr at eos");

	EXPECT_EQ_MSG(stream_size(in), 0, "stdin size is not 0");
	EXPECT_EQ_MSG(stream_size(out), 0, "stdout size is not 0");
	EXPECT_EQ_MSG(stream_size(err), 0, "stderr size is not 0");

	EXPECT_EQ_MSG(stream_is_binary(in), true, "stdin did not preserve binary mode");
	EXPECT_EQ_MSG(stream_is_binary(out), false, "stdout did not preserve ascii mode");
	EXPECT_EQ_MSG(stream_is_binary(err), true, "stderr did not preserve binary mode");

	EXPECT_EQ_MSG(stream_is_sequential(in), true, "stdin not sequential");
	EXPECT_EQ_MSG(stream_is_sequential(out), true, "stdout not sequential");
	EXPECT_EQ_MSG(stream_is_sequential(err), true, "stderr not sequential");

	EXPECT_EQ_MSG(stream_is_reliable(in), true, "stdin not reliable");
	EXPECT_EQ_MSG(stream_is_reliable(out), true, "stdout not reliable");
	EXPECT_EQ_MSG(stream_is_reliable(err), true, "stderr not reliable");

	EXPECT_EQ_MSG(stream_is_inorder(in), true, "stdin not inorder");
	EXPECT_EQ_MSG(stream_is_inorder(out), true, "stdout not inorder");
	EXPECT_EQ_MSG(stream_is_inorder(err), true, "stderr not inorder");

	EXPECT_EQ_MSG(stream_is_swapped(in), false, "stdin is swapped");
	EXPECT_EQ_MSG(stream_is_swapped(out), false, "stdout is swapped");
	EXPECT_EQ_MSG(stream_is_swapped(err), false, "stderr is swapped");

	EXPECT_EQ_MSG(stream_byteorder(in), system_byteorder(), "stdin not native byteorder");
	EXPECT_EQ_MSG(stream_byteorder(out), system_byteorder(), "stdout not native byteorder");
	EXPECT_EQ_MSG(stream_byteorder(err), system_byteorder(), "stderr not native byteorder");

	stream_set_byteorder(in, BYTEORDER_LITTLEENDIAN);
	stream_set_byteorder(out, BYTEORDER_LITTLEENDIAN);
	stream_set_byteorder(err, BYTEORDER_LITTLEENDIAN);

	EXPECT_EQ_MSG(stream_byteorder(in), BYTEORDER_LITTLEENDIAN, "stdin not little endian after explicit set");
	EXPECT_EQ_MSG(stream_byteorder(out), BYTEORDER_LITTLEENDIAN, "stdout not little endian after explicit set");
	EXPECT_EQ_MSG(stream_byteorder(err), BYTEORDER_LITTLEENDIAN, "stderr not little endian after explicit set");

	stream_set_byteorder(in, BYTEORDER_BIGENDIAN);
	stream_set_byteorder(out, BYTEORDER_BIGENDIAN);
	stream_set_byteorder(err, BYTEORDER_BIGENDIAN);

	EXPECT_EQ_MSG(stream_byteorder(in), BYTEORDER_BIGENDIAN, "stdin not big endian after explicit set");
	EXPECT_EQ_MSG(stream_byteorder(out), BYTEORDER_BIGENDIAN, "stdout not big endian after explicit set");
	EXPECT_EQ_MSG(stream_byteorder(err), BYTEORDER_BIGENDIAN, "stderr not big endian after explicit set");

	stream_set_binary(in, true);
	stream_set_binary(out, true);
	stream_set_binary(err, true);

	EXPECT_EQ_MSG(stream_is_binary(in), true, "stdin did not set binary mode");
	EXPECT_EQ_MSG(stream_is_binary(out), true, "stdout did not set binary mode");
	EXPECT_EQ_MSG(stream_is_binary(err), true, "stderr did not set binary mode");

	stream_set_binary(in, false);
	stream_set_binary(out, false);
	stream_set_binary(err, false);

	EXPECT_EQ_MSG(stream_is_binary(in), false, "stdin did not set ascii mode");
	EXPECT_EQ_MSG(stream_is_binary(out), false, "stdout did not set asii mode");
	EXPECT_EQ_MSG(stream_is_binary(err), false, "stderr did not set ascii mode");

	EXPECT_CONSTSTRINGEQ_MSG(stream_path(in), string_const(STRING_CONST("stdin://")), "stdin has wrong path");
	EXPECT_CONSTSTRINGEQ_MSG(stream_path(out), string_const(STRING_CONST("stdout://")), "stdout has wrong path");
	EXPECT_CONSTSTRINGEQ_MSG(stream_path(err), string_const(STRING_CONST("stderr://")), "stderr has wrong path");

	now = time_system();

	EXPECT_GE_MSG(stream_last_modified(in), now, "stdin has noncurrent last modification");
	EXPECT_GE_MSG(stream_last_modified(out), now, "stdout has noncurrent last modification");
	EXPECT_GE_MSG(stream_last_modified(err), now, "stderr has noncurrent last modification");

	stream_set_binary(in, true);
	stream_set_byteorder(out, BYTEORDER_LITTLEENDIAN);

	in_clone = stream_clone(in);
	out_clone = stream_clone(out);
	err_clone = stream_clone(err);

	EXPECT_NE_MSG(in_clone, 0, "Clone stdin stream returned null");
	EXPECT_NE_MSG(out_clone, 0, "Clone stdout stream returned null");
	EXPECT_NE_MSG(err_clone, 0, "Clone stderr stream returned null");

	EXPECT_CONSTSTRINGEQ_MSG(stream_path(in_clone), string_const(STRING_CONST("stdin://")), "stdin has wrong path");
	EXPECT_CONSTSTRINGEQ_MSG(stream_path(out_clone), string_const(STRING_CONST("stdout://")), "stdout has wrong path");
	EXPECT_CONSTSTRINGEQ_MSG(stream_path(err_clone), string_const(STRING_CONST("stderr://")), "stderr has wrong path");

	EXPECT_EQ_MSG(stream_is_binary(in_clone), stream_is_binary(in), "stdin did not preserve mode");
	EXPECT_EQ_MSG(stream_is_binary(out_clone), stream_is_binary(out), "stdout clone did not preserve mode");
	EXPECT_EQ_MSG(stream_is_binary(err_clone), stream_is_binary(err), "stderr did not preserve mode");

	EXPECT_EQ_MSG(stream_byteorder(in_clone), stream_byteorder(in), "stdin did not preserve byte order");
	EXPECT_EQ_MSG(stream_byteorder(out_clone), stream_byteorder(out), "stdout clone did not preserve byte order");
	EXPECT_EQ_MSG(stream_byteorder(err_clone), stream_byteorder(err), "stderr did not preserve byte order");

	stream_seek(in_clone, 1, STREAM_SEEK_BEGIN);
	stream_seek(out_clone, 1, STREAM_SEEK_CURRENT);
	stream_seek(err_clone, 1, STREAM_SEEK_END);

	EXPECT_EQ_MSG(stream_tell(in_clone), 0, "stdin clone pos is not 0");
	EXPECT_EQ_MSG(stream_tell(out_clone), 0, "stdout clone pos is not 0");
	EXPECT_EQ_MSG(stream_tell(err_clone), 0, "stderr clone pos is not 0");

	EXPECT_EQ_MSG(stream_eos(in_clone), false, "stdin clone at eos");
	EXPECT_EQ_MSG(stream_eos(out_clone), false, "stdout clone at eos");
	EXPECT_EQ_MSG(stream_eos(err_clone), false, "stderr clone at eos");

	EXPECT_EQ_MSG(stream_size(in_clone), 0, "stdin clone size is not 0");
	EXPECT_EQ_MSG(stream_size(out_clone), 0, "stdout clone size is not 0");
	EXPECT_EQ_MSG(stream_size(err_clone), 0, "stderr clone size is not 0");

	stream_deallocate(in_clone);
	stream_deallocate(out_clone);
	stream_deallocate(err_clone);

	stream_buffer_read(in);
	stream_buffer_read(out);
	stream_buffer_read(err);

	// EXPECT_GE_MSG( stream_available_read( in ), 0, "stdin invalid value for available bytes to read" );
	EXPECT_EQ_MSG(stream_available_read(out), 0, "stdout not empty as expected");
	EXPECT_EQ_MSG(stream_available_read(err), 0, "stderr not empty as expected");

	EXPECT_EQ_MSG(uint128_is_null(stream_md5(in)), true, "stdin md5 not null");
	EXPECT_EQ_MSG(uint128_is_null(stream_md5(out)), true, "stdout md5 not null");
	EXPECT_EQ_MSG(uint128_is_null(stream_md5(err)), true, "stderr md5 not null");

	EXPECT_EQ_MSG(uint256_is_null(stream_sha256(in)), true, "stdin sha256 not null");
	EXPECT_EQ_MSG(uint256_is_null(stream_sha256(out)), true, "stdout sha256 not null");
	EXPECT_EQ_MSG(uint256_is_null(stream_sha256(err)), true, "stderr sha256 not null");

	EXPECT_EQ_MSG(uint512_is_null(stream_sha512(in)), true, "stdin sha512 not null");
	EXPECT_EQ_MSG(uint512_is_null(stream_sha512(out)), true, "stdout sha512 not null");
	EXPECT_EQ_MSG(uint512_is_null(stream_sha512(err)), true, "stderr sha512 not null");

	stream_truncate(in, 256);
	stream_truncate(out, 256);
	stream_truncate(err, 256);

	stream_flush(in);
	stream_flush(out);
	stream_flush(err);

	EXPECT_EQ(0, stream_write(out, STRING_CONST("")));

	EXPECT_EQ_MSG(stream_available_read(out), 0, "stdout not empty as expected");
	EXPECT_EQ_MSG(stream_available_read(err), 0, "stderr not empty as expected");

	// Verify that eos is set when stdin is closed
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
#elif FOUNDATION_PLATFORM_POSIX
	close(STDIN_FILENO);
#endif
	EXPECT_EQ_MSG(stream_read_int8(in), 0, "stdin read when closed did not return 0");
	EXPECT_EQ_MSG(stream_eos(in), true, "stdin not at eos when closed");

	EXPECT_EQ_MSG(stream_available_read(in), 0, "stdin invalid value for available bytes to read");

	stream_deallocate(in);
	stream_deallocate(out);
	stream_deallocate(err);

	return 0;
}

DECLARE_TEST(stream, readwrite_binary) {
	char write_buffer[1024];
	char read_buffer[1024];
	stream_t* teststream;
	string_t path;
	string_const_t directory;
	string_t line;
	int i;
	uuid_t uuid_write;

	path = path_make_temporary(write_buffer, 1024);
	path = string_clone(STRING_ARGS(path));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	uuid_write = uuid_generate_random();

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_uuid(teststream, uuid_write);
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t)((void*)0));
	stream_write_string(teststream, 0, 0);

	EXPECT_EQ_MSGFORMAT(stream_tell(teststream),
	                    1025ULL + 43ULL + 112ULL + 16ULL + 45ULL + 40ULL + FOUNDATION_SIZE_POINTER * 2 + 1,
	                    "stream position not expected after writes (%" PRIsize ")", stream_tell(teststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	read_buffer[0] = 123;
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSG(read_buffer[0], 123, "buffer modified when reading zero bytes");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 1, "read one byte failed");
	EXPECT_EQ_MSG(read_buffer[0], write_buffer[0], "data not read correctly when reading one byte");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 1024, "read 1024 bytes failed");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSGFORMAT(read_buffer[i], write_buffer[i],
		                    "data not read correctly in pos %d when reading 1024 bytes", i);

	EXPECT_EQ_MSG(stream_read_bool(teststream), true, "read boolean failed");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 127, "read int8 failed");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 178, "read uint8 failed");
	EXPECT_EQ_MSG(stream_read_int16(teststream), -1234, "read int16 failed");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 45678, "read uint16 failed");
	EXPECT_EQ_MSG(stream_read_int32(teststream), -1324567, "read int32 failed");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 3245678U, "read uint32 failed");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 123456789012LL, "read int64 failed");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 8712634987126ULL, "read uint64 failed");
	EXPECT_TRUE(
	    uint128_equal(stream_read_uint128(teststream), uint128_make(913487605673245342ULL, 746598172346587ULL)));
	EXPECT_TRUE(uint256_equal(
	    stream_read_uint256(teststream),
	    uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL, 4943105789384587234ULL)));
	EXPECT_TRUE(uint512_equal(
	    stream_read_uint512(teststream),
	    uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL, 4943105789384587234ULL,
	                 798721098479438726ULL, 37843629857419827ULL, 78109235623487745ULL, 59813745190283445ULL)));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_write));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 1.0f, "read float32 failed");
	EXPECT_EQ_MSG(stream_read_float64(teststream), -1.0, "read float64 failed");

	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("test string")), "read line buffer failed");

	line = stream_read_line(teststream, '\n');
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("with some newlines")), "read line failed data");
	string_deallocate(line.str);

	line = stream_read_string(teststream);
	EXPECT_NE_MSG(line.length, 0, "read string failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("in the string")), "read string failed data");
	string_deallocate(line.str);

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
#if FOUNDATION_SIZE_POINTER == 8
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("formatted output with a null pointer 0x0000000000000000")),
	                    "read string buffer data failed");
#else
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("formatted output with a null pointer 0x00000000")),
	                    "read string buffer data failed");
#endif

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.str, 0, "Read empty string did not return null");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read line buffer failed at end of stream, read %" PRIsize " bytes",
	                    line.length);

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_determine_binary_mode(teststream, 1024);
	EXPECT_EQ_MSG(stream_is_binary(teststream), true, "Binary mode was not detected");
	stream_seek(teststream, -34, STREAM_SEEK_END);  // There is a terminating zeros at end
	stream_determine_binary_mode(teststream, 32);
	EXPECT_EQ_MSG(stream_is_binary(teststream), false, "Text mode was not detected");
	stream_seek(teststream, -32, STREAM_SEEK_END);
	stream_set_binary(teststream, true);
	stream_determine_binary_mode(teststream, 0);
	EXPECT_EQ_MSG(stream_is_binary(teststream), false, "Text mode was not detected");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 0, "write one bytes failed (wrote in read-only stream)");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 0,
	              "write 1024 bytes failed (wrote in read-only stream)");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_uuid(teststream, uuid_write);
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after writes");
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after seek");

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_TRUE(uint256_equal(stream_read_uint256(teststream), uint256_null()));
	EXPECT_TRUE(uint512_equal(stream_read_uint512(teststream), uint512_null()));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("")), "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read string buffer failed (%" PRIsize ")", line.length);
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read string buffer data failed");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_uuid(teststream, uuid_write);
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	EXPECT_EQ_MSGFORMAT(stream_tell(teststream), 1025 + 43 + 16 + 112 + 45 + 40 + FOUNDATION_SIZE_POINTER * 2,
	                    "stream position not expected after writes (%" PRIsize ")", stream_tell(teststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_TRUE(uint256_equal(stream_read_uint256(teststream), uint256_null()));
	EXPECT_TRUE(uint512_equal(stream_read_uint512(teststream), uint512_null()));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	EXPECT_EQ_MSG(stream_read_line_buffer(teststream, read_buffer, 1024, '\n').length, 0,
	              "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read string buffer failed (%" PRIsize ")", line.length);
	EXPECT_STRINGEQ_MSG(line, string_empty(), "read string buffer data failed");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));
	string_deallocate(path.str);

	return 0;
}

DECLARE_TEST(stream, readwrite_text) {
	char write_buffer[1024];
	char read_buffer[1024];
	stream_t* teststream;
	string_t path;
	string_const_t directory;
	string_t line;
	int i;
	uuid_t uuid_write;

	path = path_make_temporary(write_buffer, sizeof(write_buffer));
	path = string_clone(STRING_ARGS(path));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	uuid_write = uuid_generate_random();

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	// Make sure byte swap does not affect text mode
	stream_set_byteorder(teststream,
	                     (system_byteorder() == BYTEORDER_LITTLEENDIAN) ? BYTEORDER_BIGENDIAN : BYTEORDER_LITTLEENDIAN);

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, true);
	stream_write_endl(teststream);
	stream_write_int8(teststream, 127);
	stream_write_separator(teststream);
	stream_write_uint8(teststream, 178);
	stream_write_endl(teststream);
	stream_write_int16(teststream, -1234);
	stream_write_separator(teststream);
	stream_write_uint16(teststream, 45678);
	stream_write_endl(teststream);
	stream_write_int32(teststream, -1324567);
	stream_write_separator(teststream);
	stream_write_uint32(teststream, 3245678U);
	stream_write_endl(teststream);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_separator(teststream);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_endl(teststream);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_separator(teststream);
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_endl(teststream);
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_separator(teststream);
	stream_write_uuid(teststream, uuid_write);
	stream_write_endl(teststream);
	stream_write_float32(teststream, 1.0f);
	stream_write_separator(teststream);
	stream_write_float64(teststream, -1.0);
	stream_write_endl(teststream);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);
	stream_write_endl(teststream);

	size_t expected = 1025ULL + 74ULL + 45ULL + 194ULL + 37ULL + 33ULL + 40ULL + FOUNDATION_SIZE_POINTER * 2;
	EXPECT_EQ_MSGFORMAT(stream_tell(teststream), expected,
	                    "stream position %" PRIsize " after writes, expected %" PRIsize ": %.*s",
	                    stream_tell(teststream), expected, STRING_FORMAT(stream_path(teststream)));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	read_buffer[0] = 123;
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSG(read_buffer[0], 123, "buffer modified when reading zero bytes");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 1, "read one byte failed");
	EXPECT_EQ_MSG(read_buffer[0], write_buffer[0], "data not read correctly when reading one byte");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 1024, "read 1024 bytes failed");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSGFORMAT(read_buffer[i], write_buffer[i],
		                    "data not read correctly in pos %d when reading 1024 bytes", i);

	EXPECT_EQ_MSG(stream_read_bool(teststream), true, "read boolean failed");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 127, "read int8 failed");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 178, "read uint8 failed");
	EXPECT_EQ_MSG(stream_read_int16(teststream), -1234, "read int16 failed");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 45678, "read uint16 failed");
	EXPECT_EQ_MSG(stream_read_int32(teststream), -1324567, "read int32 failed");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 3245678U, "read uint32 failed");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 123456789012LL, "read int64 failed");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 8712634987126ULL, "read uint64 failed");
	EXPECT_TRUE(
	    uint128_equal(stream_read_uint128(teststream), uint128_make(913487605673245342ULL, 746598172346587ULL)));
	EXPECT_TRUE(uint256_equal(
	    stream_read_uint256(teststream),
	    uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL, 4943105789384587234ULL)));
	EXPECT_TRUE(uint512_equal(
	    stream_read_uint512(teststream),
	    uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL, 4943105789384587234ULL,
	                 798721098479438726ULL, 37843629857419827ULL, 78109235623487745ULL, 59813745190283445ULL)));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_write));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 1.0f, "read float32 failed");
	EXPECT_EQ_MSG(stream_read_float64(teststream), -1.0, "read float64 failed");

	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 11, "read line buffer failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("test string")), "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_NE_MSG(line.length, 0, "read line failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("with some newlines")), "read line failed data");
	string_deallocate(line.str);

	line = stream_read_string(teststream);
	EXPECT_NE_MSG(line.length, 0, "read string failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("in")), "read string failed data");
	string_deallocate(line.str);

	line = stream_read_line(teststream, '\n');
	EXPECT_NE_MSG(line.length, 0, "read line failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("the string")), "read line failed data");
	string_deallocate(line.str);

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("formatted")), "read string buffer data failed");

	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 29 + FOUNDATION_SIZE_POINTER * 2, "read line buffer failed");
#if FOUNDATION_SIZE_POINTER == 8
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("output with a null pointer 0x0000000000000000")),
	                    "read string buffer data failed");
#else
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("output with a null pointer 0x00000000")),
	                    "read string buffer data failed");
#endif

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read line buffer failed at end of stream, read %s", read_buffer);

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_determine_binary_mode(teststream, 1024);
	EXPECT_EQ_MSG(stream_is_binary(teststream), true, "Binary mode was not detected");
	stream_seek(teststream, -32, STREAM_SEEK_END);
	stream_determine_binary_mode(teststream, 32);
	EXPECT_EQ_MSG(stream_is_binary(teststream), false, "Text mode was not detected");

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	EXPECT_EQ_MSG(stream_write(teststream, longline, longlength), longlength,
	              "Failed to write long line data to stream");
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	line = stream_read_line(teststream, '\n');
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST(longline)), "Long line read failed data");
	string_deallocate(line.str);

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 0, "write one bytes failed (wrote in read-only stream)");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 0,
	              "write 1024 bytes failed (wrote in read-only stream)");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_uuid(teststream, uuid_write);
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after writes");
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after seek");

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_TRUE(uint256_equal(stream_read_uint256(teststream), uint256_null()));
	EXPECT_TRUE(uint512_equal(stream_read_uint512(teststream), uint512_null()));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSG(line.length, 0, "read string buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read string buffer failed data");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_uint256(teststream, uint256_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL));
	stream_write_uint512(teststream, uint512_make(913487605673245342ULL, 746598172346587ULL, 1984127364405918734ULL,
	                                              4943105789384587234ULL, 798721098479438726ULL, 37843629857419827ULL,
	                                              78109235623487745ULL, 59813745190283445ULL));
	stream_write_uuid(teststream, uuid_write);
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	expected = 1025 + 43 + 16 + 112 + 45 + 40 + FOUNDATION_SIZE_POINTER * 2;
	EXPECT_EQ_MSGFORMAT(stream_tell(teststream), expected,
	                    "stream position %" PRIsize " after writes, expected %" PRIsize, stream_tell(teststream),
	                    expected);
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_TRUE(uint256_equal(stream_read_uint256(teststream), uint256_null()));
	EXPECT_TRUE(uint512_equal(stream_read_uint512(teststream), uint512_null()));
	EXPECT_TRUE(uuid_equal(stream_read_uuid(teststream), uuid_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSG(line.length, 0, "read string buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read string buffer data failed");

	stream_deallocate(teststream);

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = 'a';
	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT);
	stream_truncate(teststream, 0);
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, write_buffer, sizeof(write_buffer));
	stream_write_string(teststream, STRING_CONST(" \n\t"));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	line = stream_read_string(teststream);
	string_deallocate(line.str);
	EXPECT_SIZEEQ(line.length, sizeof(write_buffer) * 8);
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	teststream->sequential = 1;
	line = stream_read_string(teststream);
	string_deallocate(line.str);
	EXPECT_SIZEEQ(line.length, sizeof(write_buffer) * 8);
	teststream->sequential = 0;
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	teststream->sequential = 1;
	line = stream_read_line_buffer(teststream, read_buffer, 64, '\n');
	EXPECT_SIZEEQ(line.length, 63);

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	stream_truncate(teststream, 0);
	stream_write_string(teststream, STRING_CONST("     word  \t\n\r  \t\t\r\n\t  whitespace\n\n"));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_skip_whitespace(teststream);
	EXPECT_NE(stream_tell(teststream), 0);
	stream_skip_whitespace(teststream);
	line = stream_read_string(teststream);
	EXPECT_CONSTSTRINGEQ(string_to_const(line), string_const(STRING_CONST("word")));
	string_deallocate(line.str);
	expected = stream_tell(teststream);
	stream_skip_whitespace(teststream);
	EXPECT_NE(stream_tell(teststream), expected);
	stream_skip_whitespace(teststream);
	stream_skip_whitespace(teststream);
	line = stream_read_string(teststream);
	EXPECT_CONSTSTRINGEQ(string_to_const(line), string_const(STRING_CONST("whitespace")));
	string_deallocate(line.str);
	stream_skip_whitespace(teststream);
	EXPECT_TRUE(stream_eos(teststream));
	stream_skip_whitespace(teststream);
	EXPECT_TRUE(stream_eos(teststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	teststream->sequential = 1;
	stream_skip_whitespace(teststream);
	EXPECT_EQ(stream_tell(teststream), 0);
	stream_skip_whitespace(teststream);
	line = stream_read_string(teststream);
	EXPECT_CONSTSTRINGEQ(string_to_const(line), string_const(STRING_CONST("word")));
	string_deallocate(line.str);
	expected = stream_tell(teststream);
	stream_skip_whitespace(teststream);
	EXPECT_EQ(stream_tell(teststream), expected);
	stream_skip_whitespace(teststream);
	stream_skip_whitespace(teststream);
	line = stream_read_string(teststream);
	EXPECT_CONSTSTRINGEQ(string_to_const(line), string_const(STRING_CONST("whitespace")));
	string_deallocate(line.str);
	stream_skip_whitespace(teststream);
	EXPECT_FALSE(stream_eos(teststream));
	stream_skip_whitespace(teststream);
	EXPECT_FALSE(stream_eos(teststream));

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	string_deallocate(path.str);

	return 0;
}

DECLARE_TEST(stream, readwrite_sequential) {
	char write_buffer[1024];
	char read_buffer[1024];
	stream_t* teststream;
	string_t path;
	string_const_t directory;
	string_t line;
	int i;

	path = path_make_temporary(write_buffer, 1024);
	path = string_clone(STRING_ARGS(path));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	teststream->sequential = 1;

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, false);
	stream_write_endl(teststream);
	stream_write_int8(teststream, 127);
	stream_write_endl(teststream);
	stream_write_uint8(teststream, 178);
	stream_write_endl(teststream);
	stream_write_int16(teststream, -1234);
	stream_write_endl(teststream);
	stream_write_uint16(teststream, 45678);
	stream_write_endl(teststream);
	stream_write_int32(teststream, -1324567);
	stream_write_endl(teststream);
	stream_write_uint32(teststream, 3245678U);
	stream_write_endl(teststream);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_endl(teststream);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_endl(teststream);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_endl(teststream);
	stream_write_float32(teststream, 1.0f);
	stream_write_endl(teststream);
	stream_write_float64(teststream, -1.0);
	stream_write_endl(teststream);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);
	stream_write_endl(teststream);

	EXPECT_EQ_MSGFORMAT(stream_tell(teststream), 1025 + 75 + 45 + 33 + 40 + FOUNDATION_SIZE_POINTER * 2,
	                    "stream position not expected after writes (%" PRIsize ") : %.*s", stream_tell(teststream),
	                    STRING_FORMAT(stream_path(teststream)));
	teststream->sequential = 0;
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	teststream->sequential = 1;

	read_buffer[0] = 123;
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSG(read_buffer[0], 123, "buffer modified when reading zero bytes");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 1, "read one byte failed");
	EXPECT_EQ_MSG(read_buffer[0], write_buffer[0], "data not read correctly when reading one byte");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 1024, "read 1024 bytes failed");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSGFORMAT(read_buffer[i], write_buffer[i],
		                    "data not read correctly in pos %d when reading 1024 bytes", i);

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean failed");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 127, "read int8 failed");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 178, "read uint8 failed");
	EXPECT_EQ_MSG(stream_read_int16(teststream), -1234, "read int16 failed");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 45678, "read uint16 failed");
	EXPECT_EQ_MSG(stream_read_int32(teststream), -1324567, "read int32 failed");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 3245678U, "read uint32 failed");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 123456789012LL, "read int64 failed");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 8712634987126ULL, "read uint64 failed");
	EXPECT_TRUE(
	    uint128_equal(stream_read_uint128(teststream), uint128_make(913487605673245342ULL, 746598172346587ULL)));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 1.0f, "read float32 failed");
	EXPECT_EQ_MSG(stream_read_float64(teststream), -1.0, "read float64 failed");

	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 11, "read line buffer failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("test string")), "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_NE_MSG(line.length, 0, "read line failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("with some newlines")), "read line failed data");
	string_deallocate(line.str);

	line = stream_read_string(teststream);
	EXPECT_NE_MSG(line.length, 0, "read string failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("in")), "read string failed data");
	string_deallocate(line.str);

	line = stream_read_line(teststream, '\n');
	EXPECT_NE_MSG(line.length, 0, "read line failed");
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("the string")), "read line failed data");
	string_deallocate(line.str);

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSGFORMAT(line.length, 9, "read string buffer failed (%" PRIsize ")", line.length);
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("formatted")), "read string buffer data failed");

	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSGFORMAT(line.length, 29 + FOUNDATION_SIZE_POINTER * 2,
	                    "read line buffer failed (%" PRIsize " read, %d expected)", line.length,
	                    28 + FOUNDATION_SIZE_POINTER * 2);
#if FOUNDATION_SIZE_POINTER == 8
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("output with a null pointer 0x0000000000000000")),
	                    "read string buffer data failed");
#else
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST("output with a null pointer 0x00000000")),
	                    "read string buffer data failed");
#endif

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read line buffer failed at end of stream, read %s", read_buffer);

	teststream->sequential = 0;
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	teststream->sequential = 1;

	EXPECT_EQ_MSG(stream_write(teststream, longline, longlength), longlength,
	              "Failed to write long line data to stream");
	teststream->sequential = 0;
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	teststream->sequential = 1;

	line = stream_read_line(teststream, '\n');
	EXPECT_STRINGEQ_MSG(line, string_const(STRING_CONST(longline)), "Long line read failed data");
	string_deallocate(line.str);

	teststream->sequential = 0;
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	teststream->sequential = 1;
	stream_set_binary(teststream, true);
	stream_determine_binary_mode(teststream, 1024);
	EXPECT_EQ_MSG(stream_is_binary(teststream), true,
	              "Text mode was detected even if stream is sequential, expected to fail");
	stream_set_binary(teststream, false);

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 0, "write one bytes failed (wrote in read-only stream)");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 0,
	              "write 1024 bytes failed (wrote in read-only stream)");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	teststream->sequential = 0;
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after writes");
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position in read-only stream not null after seek");
	teststream->sequential = 1;

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read string buffer failed (%" PRIsize ")", line.length);
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read string buffer data failed");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (i = 0; i < 1024; ++i)
		write_buffer[i] = (char)(i + 63);

	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 0), 0, "write zero bytes failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1), 1, "write one byte failed");
	EXPECT_EQ_MSG(stream_write(teststream, write_buffer, 1024), 1024, "write 1024 bytes failed");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 127);
	stream_write_uint8(teststream, 178);
	stream_write_int16(teststream, -1234);
	stream_write_uint16(teststream, 45678);
	stream_write_int32(teststream, -1324567);
	stream_write_uint32(teststream, 3245678U);
	stream_write_int64(teststream, 123456789012LL);
	stream_write_uint64(teststream, 8712634987126ULL);
	stream_write_uint128(teststream, uint128_make(913487605673245342ULL, 746598172346587ULL));
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);
	stream_write_string(teststream, STRING_CONST("test string\nwith some newlines\nin the string"));
	stream_write_endl(teststream);
	stream_write_format(teststream, STRING_CONST("formatted output with a null pointer 0x%" PRIfixPTR),
	                    (uintptr_t) nullptr);

	teststream->sequential = 0;
	EXPECT_EQ_MSGFORMAT(stream_tell(teststream), 1025 + 43 + 45 + 16 + 40 + FOUNDATION_SIZE_POINTER * 2,
	                    "stream position not expected after writes (%" PRIsize ")", stream_tell(teststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	teststream->sequential = 1;

	for (i = 0; i < 1024; ++i)
		read_buffer[i] = (char)(i + 79);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 0), 0, "read zero bytes failed");
	EXPECT_EQ_MSGFORMAT(read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0]);
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1), 0,
	              "read one byte from read-only stream did not fail as expected");
	EXPECT_EQ_MSG(read_buffer[0], 79, "buffer modified when reading from zero size read-only stream");
	EXPECT_EQ_MSG(stream_read(teststream, read_buffer, 1024), 0,
	              "read 1024 bytes from read-only stream did not fail as expected");
	for (i = 0; i < 1024; ++i)
		EXPECT_EQ_MSG(read_buffer[i], (char)(i + 79), "buffer modified when reading from zero size read-only stream");

	EXPECT_EQ_MSG(stream_read_bool(teststream), false, "read boolean did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int8(teststream), 0, "read int8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint8(teststream), 0, "read uint8 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int16(teststream), 0, "read int16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint16(teststream), 0, "read uint16 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int32(teststream), 0, "read int32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint32(teststream), 0, "read uint32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_int64(teststream), 0, "read int64 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_uint64(teststream), 0, "read uint64 did not fail as expected");
	EXPECT_TRUE(uint128_equal(stream_read_uint128(teststream), uint128_null()));
	EXPECT_EQ_MSG(stream_read_float32(teststream), 0, "read float32 did not fail as expected");
	EXPECT_EQ_MSG(stream_read_float64(teststream), 0, "read float64 did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_line_buffer(teststream, read_buffer, 1024, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line buffer did not fail as expected");
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read line buffer failed data");

	line = stream_read_line(teststream, '\n');
	EXPECT_EQ_MSG(line.length, 0, "read line did not fail as expected");

	line = stream_read_string(teststream);
	EXPECT_EQ_MSG(line.length, 0, "read string did not fail as expected");

	read_buffer[0] = 0;
	line = stream_read_string_buffer(teststream, read_buffer, 1024);
	EXPECT_EQ_MSGFORMAT(line.length, 0, "read string buffer failed (%" PRIsize ")", line.length);
	EXPECT_STRINGEQ_MSG(string(read_buffer, string_length(read_buffer)), string_empty(),
	                    "read string buffer data failed");

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));
	string_deallocate(path.str);

	return 0;
}

DECLARE_TEST(stream, readwrite_swap) {
	stream_t* teststream;
	string_t path;
	string_const_t directory;
	bool read_bool;
	int8_t read_int8;
	uint8_t read_uint8;
	int16_t read_int16;
	uint16_t read_uint16;
	int32_t read_int32;
	uint32_t read_uint32;
	int64_t read_int64;
	uint64_t read_uint64;
	uint128_t read_uint128;
	float32_cast_t cast32;
	float64_cast_t cast64;
	char buf[BUILD_MAX_PATHLEN];

	path = path_make_temporary(buf, sizeof(buf));
	path = string_clone(STRING_ARGS(path));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	stream_set_byteorder(teststream,
	                     (system_byteorder() == BYTEORDER_LITTLEENDIAN) ? BYTEORDER_BIGENDIAN : BYTEORDER_LITTLEENDIAN);
	EXPECT_EQ_MSG(stream_is_swapped(teststream), true, "swap was not set correctly");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 0x8f);
	stream_write_uint8(teststream, 0xab);
	stream_write_int16(teststream, 0xfeef);
	stream_write_uint16(teststream, 0xbaad);
	stream_write_int32(teststream, 0x12345678);
	stream_write_uint32(teststream, 0x98765432UL);
	stream_write_int64(teststream, 0x1234567890abcdefLL);
	stream_write_uint64(teststream, 0x1234567890abcdefULL);
	stream_write_uint128(teststream, uint128_make(0x1234567890abcdefULL, 0xbaadf00dfeeff00fULL));
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	stream_set_byteorder(teststream, system_byteorder());
	EXPECT_EQ_MSG(stream_is_swapped(teststream), false, "swap was not set correctly");

	read_bool = stream_read_bool(teststream);
	read_int8 = stream_read_int8(teststream);
	read_uint8 = stream_read_uint8(teststream);
	read_int16 = stream_read_int16(teststream);
	read_uint16 = stream_read_uint16(teststream);
	read_int32 = stream_read_int32(teststream);
	read_uint32 = stream_read_uint32(teststream);
	read_int64 = stream_read_int64(teststream);
	read_uint64 = stream_read_uint64(teststream);
	read_uint128 = stream_read_uint128(teststream);

	const int8_t expect_int8 = 0x8f;
	const uint8_t expect_uint8 = 0xab;
	const int16_t expect_int16 = 0xeffe;
	const uint16_t expect_uint16 = 0xadba;

	EXPECT_EQ_MSG(read_bool, true, "read boolean did not swap as expected");
	EXPECT_EQ_MSGFORMAT(read_int8, expect_int8, "read int8 did not swap as expected (%02x)", read_int8);
	EXPECT_EQ_MSGFORMAT(read_uint8, expect_uint8, "read uint8 did not swap as expected (%02x)", read_uint8);
	EXPECT_EQ_MSGFORMAT(read_int16, expect_int16, "read int16 did not swap as expected (%04x)", read_int16);
	EXPECT_EQ_MSGFORMAT(read_uint16, expect_uint16, "read uint16 did not swap as expected (%04x)", read_uint16);
	EXPECT_EQ_MSGFORMAT(read_int32, 0x78563412, "read int32 did not swap as expected (%08x)", read_int32);
	EXPECT_EQ_MSGFORMAT(read_uint32, 0x32547698UL, "read uint32 did not swap as expected (%04x)", read_uint32);
	EXPECT_EQ_MSGFORMAT(read_int64, (int64_t)0xefcdab9078563412LL, "read int64 did not swap as expected (%" PRIx64 ")",
	                    read_int64);
	EXPECT_EQ_MSGFORMAT(read_uint64, 0xefcdab9078563412ULL, "read uint64 did not swap as expected (%" PRIx64 ")",
	                    read_uint64);
	EXPECT_EQ_MSGFORMAT(read_uint128.word[0], 0xefcdab9078563412ULL,
	                    "read uint128 word 0 did not swap as expected (%" PRIx64 ")", read_uint128.word[0]);
	EXPECT_EQ_MSGFORMAT(read_uint128.word[1], 0x0ff0effe0df0adbaULL,
	                    "read uint128 word 1 did not swap as expected (%" PRIx64 ")", read_uint128.word[1]);

	cast32.fval = stream_read_float32(teststream);
	cast32.uival = byteorder_swap32(cast32.uival);
	EXPECT_EQ_MSGFORMAT(cast32.fval, 1.0f, "read float32 did not swap as expected : %f", cast32.fval);

	cast64.fval = stream_read_float64(teststream);
	cast64.uival = byteorder_swap64(cast64.uival);
	EXPECT_EQ_MSGFORMAT(cast64.fval, -1.0f, "read float64 did not swap as expected : %lf", cast64.fval);

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	EXPECT_EQ_MSG(stream_is_swapped(teststream), false, "swap was not set correctly");

	stream_write_bool(teststream, true);
	stream_write_int8(teststream, 0x8f);
	stream_write_uint8(teststream, 0xab);
	stream_write_int16(teststream, 0xfeef);
	stream_write_uint16(teststream, 0xbaad);
	stream_write_int32(teststream, 0x12345678);
	stream_write_uint32(teststream, 0x98765432UL);
	stream_write_int64(teststream, 0x1234567890abcdefLL);
	stream_write_uint64(teststream, 0x1234567890abcdefULL);
	stream_write_uint128(teststream, uint128_make(0x1234567890abcdefULL, 0xbaadf00dfeeff00fULL));
	stream_write_float32(teststream, 1.0f);
	stream_write_float64(teststream, -1.0);

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	EXPECT_EQ_MSG(stream_tell(teststream), 0, "stream position not null after seek");
	stream_set_byteorder(teststream,
	                     (system_byteorder() == BYTEORDER_LITTLEENDIAN) ? BYTEORDER_BIGENDIAN : BYTEORDER_LITTLEENDIAN);
	EXPECT_EQ_MSG(stream_is_swapped(teststream), true, "swap was not set correctly");

	read_bool = stream_read_bool(teststream);
	read_int8 = stream_read_int8(teststream);
	read_uint8 = stream_read_uint8(teststream);
	read_int16 = stream_read_int16(teststream);
	read_uint16 = stream_read_uint16(teststream);
	read_int32 = stream_read_int32(teststream);
	read_uint32 = stream_read_uint32(teststream);
	read_int64 = stream_read_int64(teststream);
	read_uint64 = stream_read_uint64(teststream);
	read_uint128 = stream_read_uint128(teststream);

	EXPECT_EQ_MSG(read_bool, true, "read boolean did not swap as expected");
	EXPECT_EQ_MSGFORMAT(read_int8, expect_int8, "read int8 did not swap as expected (%02x)", read_int8);
	EXPECT_EQ_MSGFORMAT(read_uint8, expect_uint8, "read uint8 did not swap as expected (%02x)", read_uint8);
	EXPECT_EQ_MSGFORMAT(read_int16, expect_int16, "read int16 did not swap as expected (%04x)", read_int16);
	EXPECT_EQ_MSGFORMAT(read_uint16, expect_uint16, "read uint16 did not swap as expected (%04x)", read_uint16);
	EXPECT_EQ_MSGFORMAT(read_int32, 0x78563412, "read int32 did not swap as expected (%08x)", read_int32);
	EXPECT_EQ_MSGFORMAT(read_uint32, 0x32547698UL, "read uint32 did not swap as expected (%04x)", read_uint32);
	EXPECT_EQ_MSGFORMAT(read_int64, (int64_t)0xefcdab9078563412LL, "read int64 did not swap as expected (%" PRIx64 ")",
	                    read_int64);
	EXPECT_EQ_MSGFORMAT(read_uint64, 0xefcdab9078563412ULL, "read uint64 did not swap as expected (%" PRIx64 ")",
	                    read_uint64);
	EXPECT_EQ_MSGFORMAT(read_uint128.word[0], 0xefcdab9078563412ULL,
	                    "read uint128 word 0 did not swap as expected (%" PRIx64 ")", read_uint128.word[0]);
	EXPECT_EQ_MSGFORMAT(read_uint128.word[1], 0x0ff0effe0df0adbaULL,
	                    "read uint128 word 1 did not swap as expected (%" PRIx64 ")", read_uint128.word[1]);

	cast32.fval = stream_read_float32(teststream);
	cast32.uival = byteorder_swap32(cast32.uival);
	EXPECT_EQ_MSGFORMAT(cast32.fval, 1.0f, "read float32 did not swap as expected : %f", cast32.fval);

	cast64.fval = stream_read_float64(teststream);
	cast64.uival = byteorder_swap64(cast64.uival);
	EXPECT_EQ_MSGFORMAT(cast64.fval, -1.0f, "read float64 did not swap as expected : %lf", cast64.fval);

	stream_deallocate(teststream);
	fs_remove_file(STRING_ARGS(path));
	string_deallocate(path.str);

	return 0;
}

DECLARE_TEST(stream, util) {
	stream_t* teststream;
	stream_t* dststream;
	string_t path;
	string_const_t directory;
	char buf[BUILD_MAX_PATHLEN];
	uint32_t data[256];
	size_t idata;
	stream_clone_fn clonefn;
	md5_t md5;
	sha256_t sha256;
	sha512_t sha512;

	for (idata = 0; idata < sizeof(data) / sizeof(data[0]); ++idata)
		data[idata] = random32();

	path = path_make_temporary(buf, sizeof(buf));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	for (idata = 0; idata < sizeof(data) / sizeof(data[0]); ++idata)
		stream_write_uint32(teststream, data[idata]);

	path = path_make_temporary(buf, sizeof(buf));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	dststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "dest stream '%.*s' not created", STRING_FORMAT(path));

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_copy(teststream, dststream);
	EXPECT_EQ(stream_tell(teststream), stream_tell(dststream));
	EXPECT_EQ(stream_size(teststream), stream_size(dststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_seek(dststream, 0, STREAM_SEEK_BEGIN);

	for (idata = 0; idata < sizeof(data) / sizeof(data[0]); ++idata) {
		EXPECT_EQ(stream_read_uint32(dststream), data[idata]);
	}

	stream_deallocate(dststream);
	fs_remove_file(STRING_ARGS(path));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);

	dststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_BINARY | STREAM_CREATE | STREAM_TRUNCATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "dest stream '%.*s' not created", STRING_FORMAT(path));

	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_copy(teststream, dststream);
	EXPECT_EQ(0, stream_tell(dststream));
	EXPECT_EQ(0, stream_size(dststream));
	stream_seek(teststream, 0, STREAM_SEEK_BEGIN);
	stream_seek(dststream, 0, STREAM_SEEK_BEGIN);

	for (idata = 0; idata < sizeof(data) / sizeof(data[0]); ++idata) {
		EXPECT_EQ(stream_read_uint32(dststream), 0);
	}

	stream_deallocate(dststream);

	clonefn = teststream->vtable->clone;
	teststream->vtable->clone = 0;

	dststream = stream_clone(teststream);
	EXPECT_EQ(dststream, nullptr);

	teststream->vtable->clone = clonefn;

	md5_initialize(&md5);
	md5_digest(&md5, data, sizeof(data));
	md5_digest_finalize(&md5);
	EXPECT_TRUE(uint128_equal(md5_get_digest_raw(&md5), stream_md5(teststream)));
	md5_finalize(&md5);

	sha256_initialize(&sha256);
	sha256_digest(&sha256, data, sizeof(data));
	sha256_digest_finalize(&sha256);
	EXPECT_TRUE(uint256_equal(sha256_get_digest_raw(&sha256), stream_sha256(teststream)));
	sha256_finalize(&sha256);

	sha512_initialize(&sha512);
	sha512_digest(&sha512, data, sizeof(data));
	sha512_digest_finalize(&sha512);
	EXPECT_TRUE(uint512_equal(sha512_get_digest_raw(&sha512), stream_sha512(teststream)));
	sha512_finalize(&sha512);

	stream_deallocate(teststream);
	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_TRUNCATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	stream_write_string(
	    teststream, STRING_CONST("Line endings test\nshould be digested\rto same digest\t\r\nregardless of style\n\r"));

	md5_initialize(&md5);
	md5_digest(&md5, STRING_CONST("Line endings test\nshould be digested\nto same digest\t\nregardless of style\n\n"));
	md5_digest_finalize(&md5);
	EXPECT_TRUE(uint128_equal(md5_get_digest_raw(&md5), stream_md5(teststream)));
	md5_finalize(&md5);

	sha256_initialize(&sha256);
	sha256_digest(&sha256,
	              STRING_CONST("Line endings test\nshould be digested\nto same digest\t\nregardless of style\n\n"));
	sha256_digest_finalize(&sha256);
	EXPECT_TRUE(uint256_equal(sha256_get_digest_raw(&sha256), stream_sha256(teststream)));
	sha256_finalize(&sha256);

	sha512_initialize(&sha512);
	sha512_digest(&sha512,
	              STRING_CONST("Line endings test\nshould be digested\nto same digest\t\nregardless of style\n\n"));
	sha512_digest_finalize(&sha512);
	EXPECT_TRUE(uint512_equal(sha512_get_digest_raw(&sha512), stream_sha512(teststream)));
	sha512_finalize(&sha512);

	stream_deallocate(teststream);

	return 0;
}

static void
test_stream_declare(void) {
	ADD_TEST(stream, std);
	ADD_TEST(stream, readwrite_binary);
	ADD_TEST(stream, readwrite_text);
	ADD_TEST(stream, readwrite_sequential);
	ADD_TEST(stream, readwrite_swap);
	ADD_TEST(stream, util);
}

static test_suite_t test_stream_suite = {test_stream_application,
                                         test_stream_memory_system,
                                         test_stream_config,
                                         test_stream_declare,
                                         test_stream_initialize,
                                         test_stream_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_stream_run(void);

int
test_stream_run(void) {
	test_suite = test_stream_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_stream_suite;
}

#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
