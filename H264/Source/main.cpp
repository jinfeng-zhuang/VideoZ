#include "main.h"

const char* filename = "test.h264";

int main(void)
{
	int ret;
	FILE *fp;
	unsigned int file_size;
	unsigned char *content;
	unsigned int bytes_read;
	unsigned int length;
	unsigned char *buffer;
	enum nal_unit_type type;

	ret = fopen_s(&fp, filename, "rb");
	if ((0 != ret) || (NULL == fp)) {
		printf("%s not exist\n", filename);
		goto file_not_exist;
	}

	fseek(fp, 0, SEEK_END);

	file_size = ftell(fp);

	content = (unsigned char *)malloc(file_size);
	if (NULL == content) {
		printf("no enough space for file : %d bytes\n", file_size);
		goto malloc_failed;
	}

	fseek(fp, 0, SEEK_SET);

	bytes_read = fread(content, sizeof(unsigned char), file_size, fp);
	if (bytes_read != file_size) {
		printf("can't read enough data from %s, supose %d, but %d\n", filename, file_size, bytes_read);
		goto file_read_failed;
	}

	nalu_parse_all(content, file_size);

	while (0 == nalu_next(&type, &buffer, &length)) {
#if 0
		if (NULL != g_nalu_callback[type])
			g_nalu_callback[type](buffer, length);
#endif
        printf("nalu type %x\n", type);
	}

	free(content);
	fclose(fp);

	return 0;

file_read_failed:
	free(content);
malloc_failed:
	fclose(fp);
file_not_exist:
	return 0;
}
