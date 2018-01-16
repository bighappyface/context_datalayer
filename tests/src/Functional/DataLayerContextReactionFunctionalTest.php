<?php

namespace Drupal\Tests\context_datalayer\Functional;

use Drupal\Tests\BrowserTestBase;

/**
 * DataLayer Context Reaction Functional Tests.
 *
 * @group context_datalayer
 */
class DataLayerContextReactionFunctionalTest extends BrowserTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = [
    'context',
    'context_ui',
    'datalayer',
    'context_datalayer',
  ];

  /**
   * Tests dataLayer output.
   */
  public function testDataLayerOutput() {
    // Setup assert session.
    $assert = $this->assertSession();
    // Login.
    $this->drupalLogin($this->rootUser);
    // Create context.
    $this->drupalPostForm('admin/structure/context/add', ['label' => 'datalayer_test_context', 'name' => 'datalayer_test_context'], t('Save'));
    // Add condition for authenticated users.
    $this->drupalGet('admin/structure/context/datalayer_test_context/condition/add/user_role');
    $this->drupalPostForm('admin/structure/context/datalayer_test_context', ['conditions[user_role][roles][authenticated]' => 'authenticated'], t('Save and continue'));
    // Add datalayer reaction.
    $this->drupalGet('admin/structure/context/datalayer_test_context/reaction/add/datalayer');
    $this->drupalPostForm(
      'admin/structure/context/datalayer_test_context',
      [
        'reactions[datalayer][datalayer_key]' => 'foo',
        'reactions[datalayer][datalayer_value]' => 'bar',
        'reactions[datalayer][datalayer_type]' => 'int',
      ],
      t('Save and continue')
    );
    // Verify field values.
    $this->drupalGet('admin/structure/context/datalayer_test_context');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_key]', 'foo');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_value]', 'bar');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_type]', 'int');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_overwrite]', '');
    // Verify dataLayer addition.
    $assert->pageTextContains('var dataLayer = [{"drupalLanguage":"en","drupalCountry":"","siteName":"Drupal","userUid":"1","foo":"bar"}];');
    // // Set enable overwrite.
    $this->drupalPostForm(
      'admin/structure/context/datalayer_test_context',
      [
        'reactions[datalayer][datalayer_key]' => 'foo',
        'reactions[datalayer][datalayer_value]' => 'bar',
        'reactions[datalayer][datalayer_type]' => 'int',
        'reactions[datalayer][datalayer_overwrite]' => '1',
      ],
      t('Save and continue')
    );
    // Verify field values.
    $this->drupalGet('admin/structure/context/datalayer_test_context');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_key]', 'foo');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_value]', 'bar');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_type]', 'int');
    $assert->fieldValueEquals('reactions[datalayer][datalayer_overwrite]', '1');
    // Verify dataLayer overwrite.
    $assert->pageTextContains('var dataLayer = [{"foo":"bar"}];');
  }

}
