#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_string.h>

#include <ngx_http.h>

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
	ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "a new requset %N");

	if(!(r->method &(NGX_HTTP_GET)))
	{
		return NGX_HTTP_NOT_ALLOWED;
	}

	ngx_int_t rc = ngx_http_discard_request_body(r);
	if(rc != NGX_OK)
	{
		return rc;
	}

	ngx_str_t reponse = ngx_string("Hello World!");

	r->headers_out.status = NGX_HTTP_OK;
	r->headers_out.content_length_n = reponse.len;

	ngx_table_elt_t *h = ngx_list_push( & r->headers_out.headers);
	h->hash = 1;

#define str2ngx_string(s,str) \
	s.len = sizeof(str) - 1;\
	s.data = (u_char *)str;

	str2ngx_string(h->key,"ModuleName");
	str2ngx_string(h->value,"Mytest");


#undef str2ngx_string

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

	ngx_log_error(NGX_LOG_ALERT, cf->log, 0, "register handler, %s %d", __FILE__, __LINE__);

	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_mytest_handler;
	return NGX_CONF_OK;
}

static ngx_command_t commands[] = {
		{
			ngx_string("mytest"),
			NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LMT_CONF|NGX_CONF_NOARGS,
			ngx_http_mytest,
			NGX_HTTP_LOC_CONF_OFFSET,
			0,
			NULL
		},
		ngx_null_command,
};


static ngx_http_module_t module_ctx = {
		NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,NULL
};


ngx_module_t ngx_http_mytest1_module ={
		NGX_MODULE_V1,
		&module_ctx,
		commands,
		NGX_HTTP_MODULE,
		NULL,NULL,NULL,NULL,
		NULL,NULL,NULL,

		NGX_MODULE_V1_PADDING
};
