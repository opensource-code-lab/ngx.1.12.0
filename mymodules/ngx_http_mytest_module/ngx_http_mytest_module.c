#include <ngx_http.h>
#include <ngx_string.h>
#include <ngx_file.h>

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
	if(!(r->method &(NGX_HTTP_GET)))
	{
		return NGX_HTTP_NOT_ALLOWED;
	}

	ngx_int_t rc = ngx_http_discard_request_body(r);
	if(rc != NGX_OK)
	{
		return rc;
	}
	ngx_str_t type = ngx_string("text/plain");
	ngx_str_t reponse = ngx_string("Hello World!");

	r->headers_out.status = NGX_HTTP_OK;
	r->headers_out.content_type = type;
	r->headers_out.content_length_n = reponse.len;

	ngx_table_elt_t *h = ngx_list_push( & r->headers_out.headers);
	h->hash = 1;
#define str2ngx_string(s,str) \
	s.len = sizeof(str) - 1;\
	s.data = (u_char *)str;

	str2ngx_string(h->key,"ModuleName");
	str2ngx_string(h->value,"Mytest");

//	h->key.len = sizeof("ModuleName") - 1;
//	h->key.data = "ModuleName";
//	h->key  =   ngx_string("ModuleName");
//	h->value = ngx_string("Mytest");
#undef str2ngx_string
//	str2ngx_string(h->value,"Mytest1"); --- error call macro

	rc = ngx_http_send_header(r);
	if(rc == NGX_ERROR || rc > NGX_OK || r->header_only)
	{
		return rc;
	}

	ngx_buf_t *b;
	b = ngx_create_temp_buf(r->pool, reponse.len);
	if(b == NULL)
	{
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}

	ngx_memcpy(b->pos, reponse.data, reponse.len);
	b->last = b->pos + reponse.len;
	b->last_buf = 1;


	ngx_chain_t out;
	out.buf = b;
	out.next = NULL;

	return ngx_http_output_filter(r,&out);
}

static char * ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_mytest_handler;
	return NGX_CONF_OK;
}

static ngx_int_t ngx_http_mysendfile_handler(ngx_http_request_t *r)
{
	if(!(r->method &(NGX_HTTP_GET)))
	{
		return NGX_HTTP_NOT_ALLOWED;
	}

	ngx_int_t rc = ngx_http_discard_request_body(r);
	if(rc != NGX_OK)
	{
		return rc;
	}

	ngx_buf_t *b;
	b = ngx_palloc(r->pool, sizeof(*b));

	char * filename = "/data/nginx-1.12.0/mymodules/ngx_http_mytest_module/ngx_http_mytest_module.c";
	b->in_file = 1;
	b->file = ngx_palloc(r->pool, sizeof(*(b->file)));
	b->file->fd = ngx_open_file(filename, NGX_FILE_RDONLY|NGX_FILE_NONBLOCK, NGX_FILE_OPEN, 0);
	b->file->log = r->connection->log;
	b->file->name.data = (unsigned char *)filename;
	b->file->name.len = strlen(filename);

	if(b->file->fd <=0 )
		return NGX_HTTP_NOT_FOUND;

	if(ngx_file_info(filename, &b->file->info))
	{
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	b->file_pos = 0;
	b->file_last  = b->file->info.st_size;

//	ngx_str_t type = ngx_string("text/plain");
	r->headers_out.status = NGX_HTTP_OK;
//	r->headers_out.content_type = type;
	r->headers_out.content_length_n = b->file->info.st_size;
	ngx_http_send_header(r);


	ngx_chain_t out;
	out.buf = b;
	out.next = NULL;

	return ngx_http_output_filter(r,&out);
}


static char * ngx_http_mysendfile(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_mysendfile_handler;
	return NGX_CONF_OK;
}

static ngx_command_t ngx_http_mytest_commands[] ={
	{
			ngx_string("mytest"),
			NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
			ngx_http_mytest,
			NGX_HTTP_LOC_CONF_OFFSET,
			0,
			NULL
	},
	{
			ngx_string("mysendfile"),
			NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
			ngx_http_mysendfile,
			NGX_HTTP_LOC_CONF_OFFSET,
			0,
			NULL
	},
	ngx_null_command,
};

typedef struct{
	ngx_str_t my_str;
	ngx_int_t my_num;
	ngx_flag_t my_flag;
	size_t my_size;
	ngx_array_t* my_str_array;
	ngx_array_t* my_keyval;
	off_t my_off;
	ngx_msec_t my_msec;
	time_t my_sec;
	ngx_bufs_t my_buffs;
	ngx_uint_t my_enum_seq;
	ngx_uint_t my_bitmask;
	ngx_uint_t my_access;
	ngx_path_t* my_path;
	ngx_http_upstream_conf_t upstream;
}ngx_http_mytest_conf_t;

static void* ngx_http_mytest_create_loc_conf(ngx_conf_t *cf)
{
	ngx_http_mytest_conf_t *mycn;
	mycn = (ngx_http_mytest_conf_t*) ngx_pcalloc(cf->pool, sizeof(*mycn));
	if(mycn == NULL)
		return NULL;
	mycn->my_flag = NGX_CONF_UNSET;
	mycn->my_num = NGX_CONF_UNSET;
	mycn->my_str_array = NGX_CONF_UNSET_PTR;
	mycn->my_keyval = NULL;
	mycn->my_off = NGX_CONF_UNSET;
	mycn->my_msec = NGX_CONF_UNSET_MSEC;
	mycn->my_sec = NGX_CONF_UNSET;
	mycn->my_size = NGX_CONF_UNSET_SIZE;

	mycn->upstream.connect_timeout = 60000;
	mycn->upstream.send_timeout = 60000;
	mycn->upstream.read_timeout= 60000;
	mycn->upstream.store_access = 0600;

	mycn->upstream.buffering = 0;
	mycn->upstream.bufs.num = 8;
	mycn->upstream.bufs.size = ngx_pagesize;
	mycn->upstream.buffer_size = ngx_pagesize;
	mycn->upstream.busy_buffers_size = 2 * ngx_pagesize;
	mycn->upstream.temp_file_write_size = 2 * ngx_pagesize;
	mycn->upstream.max_temp_file_size = 1024 * 1024 * 1024;

	mycn->upstream.hide_headers = NGX_CONF_UNSET_PTR;
	mycn->upstream.hide_headers_hash = NGX_CONF_UNSET_PTR;

	return mycn;
}

static ngx_str_t  ngx_http_proxy_hide_headers[] ={
    ngx_string("Date"),
    ngx_string("Server"),
    ngx_string("X-Pad"),
    ngx_string("X-Accel-Expires"),
    ngx_string("X-Accel-Redirect"),
    ngx_string("X-Accel-Limit-Rate"),
    ngx_string("X-Accel-Buffering"),
    ngx_string("X-Accel-Charset"),
    ngx_null_string
};

static char * ngx_http_mytest_merge_loc_conf(ngx_conf_t cf, void* parent, void* child)
{
	ngx_http_mytest_conf_t *prev = parent;
	ngx_http_mytest_conf_t *conf = child;

	ngx_hash_init_t hash;
	hash.max_size = 100;
	hash.bucket_size = 1024;
	hash.name = "proxy_headers_hash";

	if(ngx_http_upstream_hide_headers_hash(cf,
			&conf->upstream,&prev->upstream,
			ngx_http_proxy_hide_headers, &hash) != NGX_OK)
	{
		return NGX_CONF_ERROR;
	}

	return NGX_CONF_OK;
}

static ngx_http_module_t ngx_http_mytest_module_ctx = {
		NULL,NULL,NULL,NULL,
		NULL,NULL,
		ngx_http_mytest_create_loc_conf,
		ngx_http_mytest_merge_loc_conf
};

ngx_module_t ngx_http_mytest_module = {
		NGX_MODULE_V1,
		&ngx_http_mytest_module_ctx,
		ngx_http_mytest_commands,
		NGX_HTTP_MODULE,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NGX_MODULE_V1_PADDING
};





