#include <uk/asm/efi.h>

static void _init_dtb(void *dtb_pointer)
{
    efi_system_table_t *efi_system_table;
    void *pagetables_start_addr, *efi_config_table;

    struct bootparamsinterface *a2 = dtb_pointer;
    efi_system_table = a2->systemtable;
    efi_config_table = efi_system_table->tables;
    efi_config_parse_tables(efi_config_table, efi_system_table->nr_tables, arch_tables);
}

static int match_config_table(const efi_guid_t *guid,
				     unsigned long table,
				     const efi_config_table_type_t *table_types)
{
	int i;

	for (i = 0; efi_guidcmp(table_types[i].guid, NULL_GUID); i++) {
		if (efi_guidcmp(*guid, table_types[i].guid))
			continue;

		if (!efi_config_table_is_usable(guid, table)) {
			if (table_types[i].name[0])
				pr_cont("(%s=0x%lx unusable) ",
					table_types[i].name, table);
			return 1;
		}

		*(table_types[i].ptr) = table;
		if (table_types[i].name[0])
			pr_cont("%s=0x%lx ", table_types[i].name, table);
		return 1;
	}

	return 0;
}

int efi_config_parse_tables(const efi_config_table_t *config_tables,
				   int count,
				   const efi_config_table_type_t *arch_tables)
{
	const efi_config_table_64_t *tbl64 = (void *)config_tables;
	const efi_config_table_32_t *tbl32 = (void *)config_tables;
	const efi_guid_t *guid;
	unsigned long table;
	int i;

	pr_info("");
	for (i = 0; i < count; i++) {
		if (efi_enabled(EFI_64BIT)) {
			guid = &tbl64[i].guid;
			table = tbl64[i].table;
		} else {
			guid = &tbl32[i].guid;
			table = tbl32[i].table;
		}

		if (!match_config_table(guid, table, common_tables) && arch_tables)
			match_config_table(guid, table, arch_tables);
	}
}